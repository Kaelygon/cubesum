//oeis A157026
//
//g++ -std=c++20 ./cubesum.cpp -o cubesum
//
//Finds N for N^3=A^3+B^3+C^3 where N,A,B,C are unique positive integers
//see prime.cfg
//
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>

using namespace std;

#include "./include/kael128i.h"

#include <condition_variable>
#include <mutex>

std::mutex tdmtx;
std::condition_variable cv;
bool threadready = true;

//public thread variables, each thread accesses own element of vector
vector <bool> active_thread;
vector <__uint64_t> found; 
vector <__uint64_t> last_compute;
atomic <__uint32_t> update_rate = 256;
bool threads_active=1;
string input;


void readinput(void)
{	
	string strtmp="";
	while(1){ 
		getline(cin, strtmp);
		if(!threads_active){break;}
		input=strtmp;
	}
}

void bgtasks(void)
{
	thread inpthread = thread(readinput);
	
	while(threads_active==1){

		if ( input=="q" || input=="quit" )
		{
			std::cout << "#User exit\n";
			std::cout << "#Closing threads\n";

			unique_lock<mutex> lck(tdmtx);
			threadready=0;
				//do stuff
				update_rate=1;
				for(uint i=0;i<active_thread.size();i++){
					active_thread[i]=0;
				}
			threadready=1;
			lck.unlock();
			cv.notify_one();
			threads_active=0;

		}
		this_thread::sleep_for(100ms);

		//check if there's active threads
		bool no_threads=1;
		for(uint i=0;i<active_thread.size();i++){
			if(active_thread[i]==1){
				no_threads=0;
			};
		}
		if(no_threads==1){
			cout << "#Threads finished\n";
			cout << "#Press enter\n";
			threads_active=0;
		}

	}
	
	inpthread.detach(); //will not free memory due to cin
	return;
}

//threads compute
void compute(const int tdid, const int tdcount, const  __uint128_t tdsrt, const  __uint128_t tdtarg3, const string tfil)
{
	ofstream tdfile;
	tdfile.open(tfil+".txt", ios::out | ios::app);

	//init vars
	__uint128_t a3,b3,ab3,a,b,c;
	__uint128_t ctarg,r0;

	__uint32_t updateinc=0;
	for(a=tdsrt;true;a+=tdcount){ //apply offsets per thread
	// A
		updateinc++;

		a3=a*a*a;
		if(3*a3>tdtarg3){break;}
		
		if( updateinc>update_rate ){
			updateinc=0;

			//make sure writing is not in progress
  			unique_lock<mutex> lck(tdmtx);
			while (!threadready) cv.wait(lck);
				bool isactive = active_thread[tdid];

			//check if closing threads
			if(isactive==0){
				last_compute[tdid]=a-tdcount;
				return;
			}

			tdfile << (string)("#a[" + to_string(tdid) + "]: " + to_string(uint64_t(a)) + "\n") << flush;
		}
		
		for( b=a+1; true; b++){
		// B

			b3=b*b*b;
			ab3=a3+b3;
			if(a3+2*b3>tdtarg3){break;}
			
			//C
				ctarg=tdtarg3-ab3; //ctarg==c*c*c

				//quadratic convergence
				r0 = 1<<( iclz_ui128(ctarg) / 3); //1<<ceil((128-CLZ+2) / 3)
				do{
					c = r0;
					r0 = (2*c + ctarg/(c*c))/3 ;
				}
				while (r0 < c);
			//EOF C

			if((ab3+c*c*c)!=tdtarg3){continue;}

			found[tdid]++;
			string write_out =
				ui128tos(tdtarg3)
				+ " = "
				+ to_string(uint64_t(a)) + "^3+"
				+ to_string(uint64_t(b)) + "^3+"
				+ to_string(uint64_t(c)) + "^3"
				+ "\n"
			;
			tdfile << write_out << flush;
			
		}
	}

	tdfile.close();
	active_thread[tdid]=0;
	last_compute[tdid]=a;
	return;
}


int main( int argc, char *argv[] ){
//finds target for target^3=A^3+B^3+C^3 	//check set [target,start] 	//time increases proportionately s^2 

	//BOF config
	
	int tc=1; //thread count
	__uint128_t start=0; //start 
	__uint128_t target=1000; //target

	string thread_file_suffix = "x";
	string work_directory = "./";
	string config = work_directory+"prime.cfg";	//config file
	string progress_file = work_directory+"lastSolve.txt";
	bool clear_file=0;
	update_rate=256;
	
	if( argc>1 ){
		config = argv[1];
	}


	if(config!=""){
		ifstream config_file (config);
		stringstream tmp;
		tmp << config_file.rdbuf();
		string config_string = tmp.str();

		//parser
		string word="";
		for(uint i=0; (config_string[i]!='?') && (i<config_string.size()) ;i++){ //word before '='
			if(config_string[i]!='='){
				word+=config_string[i];
				if(config_string[i]=='#'){//skip comments
					while(config_string[i]!='\n'){i++;}word="";
				}
			}else{
				string value="";
				i++;
				for(i=i;i<config_string.size();i++){ //value after '=' or '#'
					if(config_string[i]=='#'){//skip comments
						while(config_string[i]!='\n'){i++;}break;
					}
					if(config_string[i]!='\n'){
						value+=(char)config_string[i];
					}else{break;}
				}

				//only few elements so this unelegant solution will do. std::map?
				if		(	word=="thread_count" ){
					tc=stoi(value); 
					if(tc<=0){tc=1;}
				}else if(	word=="start"	){
					start=stoi(value); 
					if(start<0){start=0;}
				}else if(	word=="target" ){
					target=stoi(value); 
					if(target<=0){target=1;}
				}else if(	word=="progress_file"){
					progress_file=value;
				}else if(	word=="thread_file_suffix" ){
					thread_file_suffix=value;
				}else if(	word=="update_rate" ){
					update_rate=stoi(value); 
					if(update_rate<=0){update_rate=1;}
				}else if(	word=="work_directory" ){
					work_directory=value;
				}else if(	word=="clear_file" ){
					clear_file=stoi(value);
				}
				cout << "#" << word << "=" << value << "\n";
				word="";
			}
		}
		
		
	}
	//EOF config

	if(clear_file==1){
		for(int i=0;i<tc;i++){
			ofstream thread_file;
			thread_file.open(work_directory+to_string(i)+thread_file_suffix+".txt", ios::out);
			thread_file << "";
			thread_file.close();
		}
	}
	
	if(progress_file!=""){//progress file
		ifstream in_progress_file;
		in_progress_file.open(work_directory+progress_file, ios::in);

		stringstream tmp;
		tmp << in_progress_file.rdbuf();
		in_progress_file.close();

		string progress_string = tmp.str();

		string word="";
		for(int i=0;progress_string[i];i++){ //word before '='
			if(progress_string[i]!='\n'){
				word+=progress_string[i];
			}
		}
		if(word!=""){
			start=stoi(word);
		}
		cout << "#progress_file=" << to_string((uint64_t)start) << "\n";
	}
	

	//calculate A range offsets per thread
	static const __uint128_t tdsrt=start; 	//A start

	static const __uint128_t target3=target*target*target;

	//BOF tasks
	thread *task = new thread[tc];
	thread bgthread;

	for(int i=0;i<tc;i++){  //spawn threads
		found.push_back(0);
		active_thread.push_back(1);
		last_compute.push_back(0);
		task[i] = thread(
			compute,			//thread function
			i,					//thread id
			tc,					//thread count and A increment
			tdsrt+i,			//thread start
			target3,			//N^3
			work_directory+to_string(i)+thread_file_suffix		//thread file name
		);
	}

	//background tasks
	bgthread = thread(bgtasks);

	for(int i=0;i<tc;i++){
		//synchronize threads:
		task[i].join();
	}
	
  	delete [] task;
	//EOF tasks

	__uint64_t foundsum=0;
	__uint64_t lastsolve=-1;
	for(int i=0;i<tc;i++){
		foundsum+=found[i];
		if( lastsolve > last_compute[i] ){
			lastsolve = last_compute[i];
		}
	}

	std::cout << "#found: " << foundsum << "\n";
	std::cout << "#last Solve: " << lastsolve << "\n";

	ofstream in_progress_file;
	in_progress_file.open(work_directory+progress_file, ios::out);
	in_progress_file << to_string(lastsolve);
	in_progress_file.close();

  	found.clear();
  	last_compute.clear();
  	active_thread.clear();

	bgthread.join();
	return 0;
}

