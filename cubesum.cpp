//oeis A157026
//
//g++ -std=c++20 ./cubesum.cpp -o cubesum
//
//Finds N for N^3=A^3+B^3+C^3 where N,A,B,C are unique positive integers
//see prime.cfg
//
#include <thread>
#include <unistd.h>
#include <atomic>

using namespace std;

#include "./include/kael128i.h"

#define logSearch 1

#include <condition_variable>
#include <mutex>

std::mutex tdmtx;
std::condition_variable cv;
bool threadready = true;

//public thread variables, each thread accesses own element of vector
vector <bool> active_thread;
vector <__uint64_t> found; 
vector <__uint64_t> last_compute;
atomic <__uint64_t> update_rate = 256;
bool threads_active=1;
string input;


void readinput(void)
{	
	while(1){ 
		getline(cin, input);
		if(input=="" && !threads_active){break;}
	}
}

void runinput(void)
{
	thread waitcmd = thread(readinput);
	
	while(threads_active==1){

		if ( input=="q" || input=="quit" )
		{
			std::cout << "#User exit\n";
			std::cout << "#Closing threads\n";

			// Wait until main() sends data
			unique_lock<std::mutex> lck(tdmtx);
			threadready=0;
				//do stuff
				update_rate=1;
				for(int i=0;i<active_thread.size();i++){
					active_thread[i]=0;
				}
			threadready=1;
			lck.unlock();
			cv.notify_one();
			threads_active=0;

		}
		usleep(100000); //sleep 100ms

		//check if there's active threads
		bool no_threads=1;
		for(int i=0;i<active_thread.size();i++){
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
	
	waitcmd.join();
	return;
}


//compute,i,tc,tdsrt,tdinc,target3
//thread(compute,tdsrt,tdinc,target3)
void compute(const int tdid, const int tdcount, const  __uint128_t tdsrt, const  __uint128_t tdinc, const  __uint128_t target, const  __uint128_t tdtarg3, const string work_directory, const string thread_file_suffix)
{
	ofstream tdfile;
	tdfile.open(work_directory+to_string(tdid)+thread_file_suffix+".txt", ios::out | ios::app);

	//init vars
	__uint128_t a3,b3,c3,ab3,abc3,a,b,c;
	int inci;

	#if !logSearch
		__uint128_t upb,cur;
	#endif

	for(a=tdsrt;true;a+=tdinc){// A //apply offsets per thread

		a3=a*a*a;
		if(a3>tdtarg3){break;}

		if( fastmod(a-tdid,update_rate)==0 ){

			//make sure writing is not in progress
  			unique_lock<mutex> lck(tdmtx);
			while (!threadready) cv.wait(lck);
				bool isactive = active_thread[tdid];
				__uint64_t last=last_compute[tdid]; 

			//check if closing threads
			if(isactive==0){
				last=a-tdinc;
				return;
			}

			tdfile << (string)("#a[" + to_string(tdid) + "]: " + ui128tos(a) + "\n") << flush;
		}
		
		for( b=a+1; b<tdtarg3+1; b++){// B

			b3=b*b*b;
			ab3=a3+b3;

			if(ab3>tdtarg3){break;}
			
			//C
				c=b+1;
				c3=c*c*c;
				if((ab3+c3)>tdtarg3){break;}

				//search algorithms for C	
				#if logSearch
					//log search c //faster with large numbers, target>300000, varies cpu to cpu
					__uint128_t tdtarget=tdtarg3-ab3;
					for(inci=ui128log10(c); inci>=0; inci-=1){ //test increment 10^i , 10^(i-1) ... 10^2, 10^1

						__uint128_t inc = ui128pow10(inci);
						while(c*c*c<tdtarget){
							c+=inc;
						}
						c-=inc;
					}
					while(c*c*c<tdtarget){ //1 increment
						c+=1;
					}

					c3=c*c*c;
					abc3=ab3+c3;
				#else
					//binary search c
					upb = target-1;	//upper bound
					cur = (c+upb)/2;	//current estimate
					while(c <= upb)
					{
						c3=cur*cur*cur;
						abc3=ab3+c3;
						if(abc3<tdtarg3)
							c = cur+1;
						else if(abc3==tdtarg3)
						{
							break;
						}else{
							upb = cur-1;
						}
						cur = (c+upb)/2;
					}
				#endif

				if(abc3!=tdtarg3){continue;}
				else{
					found[tdid]++;
					string write_out =
						  ui128tos(tdtarg3)
						+ " = "
						+ ui128tos(target)
						+ "^3 = "
						+ ui128tos(a) + "^3+"
						+ ui128tos(b) + "^3+"
						+ ui128tos(c) + "^3"
						+ "\n"	
					;
					tdfile << write_out << flush;
				}
			//EOF C


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
		for(int i=0;i<config_string.size();i++){ //word before '='
			if(config_string[i]!='='){
				word+=config_string[i];
				if(config_string[i]=='#'){//skip comments
					while(config_string[i]!='\n'){i++;}word="";
				}
			}else{
				string value="";
				i++;
				for(i;i<config_string.size();i++){ //value after '=' or '#'
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
				}else if(	word=="end" ){
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
	}else{
		start=0;
	}
	

	//calculate A range offsets per thread
	static const __uint128_t tdsrt=start; 	//A start
	static const __uint128_t tdinc=tc; 		//A increment

	static const __uint128_t target3=target*target*target; //(s+1)^3

	//BOF tasks
	thread *task = new thread[tc];
	thread runcmd;

	for(int i=0;i<tc;i++){  // spawn threads
		found.push_back(0);
		active_thread.push_back(1);
		last_compute.push_back(0);
		task[i] = thread(
			compute,			//thread function
			i,					//thread id
			tc,					//thread count
			tdsrt+i,			//thread start
			tdinc,				//thread A incrementor
			target,				//
			target3,			//
			work_directory,		//
			thread_file_suffix	//
		);
	}

	//wait for commands
	runcmd = thread(runinput);

	for(int i=0;i<tc;i++){
		// synchronize threads:
		task[i].join();                // pauses until first finishes
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

	runcmd.join();
	exit(0);
}

