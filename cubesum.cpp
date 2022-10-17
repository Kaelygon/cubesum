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
#include <unistd.h>
#include <atomic>

using namespace std;

#include "./include/kael128i.h"

#define searchAlg 3

//0=binary
//1=base 2
//2=base 10
//3=base 16	

#if searchAlg==1
	#include "./include/tabp2.h"
#elif searchAlg==2
	#include "./include/tabp10.h"
#elif searchAlg==3
	#include "./include/tabp16.h"
#endif

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
	string strtmp="";
	while(1){ 
		getline(cin, strtmp);
		if(!threads_active){break;}
		input=strtmp;
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
	
	waitcmd.detach();
	return;
}


//compute,i,tc,tdsrt,tdinc,target3
//thread(compute,tdsrt,tdinc,target3)
void compute(const int tdid, const int tdcount, const  __uint128_t tdsrt, const  __uint128_t tdinc, const  __uint128_t tdtarg, const  __uint128_t tdtarg3, const string work_directory, const string thread_file_suffix)
{
	ofstream tdfile;
	tdfile.open(work_directory+to_string(tdid)+thread_file_suffix+".txt", ios::out | ios::app);

	//init vars
	__uint128_t a3,b3,c3,ab3,abc3,a,b,c;
	int inci;

	#if searchAlg==1||searchAlg==2||searchAlg==3
		__uint128_t ctarg,inc;
	#elif searchAlg==0
		__uint128_t upb,cur;
	#endif

	__uint64_t updateinc=0;
	for(a=tdsrt;true;a+=tdinc){// A //apply offsets per thread
		updateinc++;

		a3=a*a*a;
		if(3*a3>tdtarg3){break;}
		if( updateinc==update_rate ){
			updateinc=0;

			//make sure writing is not in progress
  			unique_lock<mutex> lck(tdmtx);
			while (!threadready) cv.wait(lck);
				bool isactive = active_thread[tdid];

			//check if closing threads
			if(isactive==0){
				last_compute[tdid]=a-tdinc;
				return;
			}

			tdfile << (string)("#a[" + to_string(tdid) + "]: " + ui128tos(a) + "\n") << flush;
		}
		
		for( b=a+1; b<tdtarg3+1; b++){// B

			b3=b*b*b;
			ab3=a3+b3;

			if(a3+2*b3>tdtarg3){break;}
			
			//C
				c=b+1;
				c3=c*c*c;
				__uint128_t asdf=ab3+c3;
				if((ab3+c3)>tdtarg3){break;}
				
				//search algorithms for C	
				#if searchAlg==0
					//binary search c
					upb = tdtarg-1;	//upper bound
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
				#elif searchAlg==1
					//bitwise base 2 search
					ctarg=tdtarg3-ab3;
					for(inci=ui128log2(c)+1; inci>=0; inci-=1){ //test increment 2^i , 2^(i-1) ... 2^2, 2^1, 2^0
						inc = tabp2[inci];
						c=c|inc;
						if(c*c*c>ctarg){
							c=c^inc;
						}
					}
					c+=1;
					c3=c*c*c;
					abc3=ab3+c3;
				#elif searchAlg==2
					//base 10 search c //faster than binary search with large numbers, target>150 000, varies cpu to cpu
					ctarg=tdtarg3-ab3;
					for(inci=ui128log10(c)+1; inci>=0; inci-=1){ //test increment 10^i , 10^(i-1) ... 10^2, 10^1, 10^0

						inc = tabp10[inci];
						while(c*c*c<ctarg){
							c+=inc;
						}
						c-=inc;
					}
					c+=inc;

					c3=c*c*c;
					abc3=ab3+c3;
				#elif searchAlg==3
					//base 16 search c
					ctarg=tdtarg3-ab3;
					for(inci=ui128log16(c)+1; inci>=0; inci-=1){ //test increment 16^i , 16^(i-1) ... 16^2, 16^1, 16^0

						inc = tabp16[inci];
						while(c*c*c<ctarg){
							c+=inc;
						}
						c-=inc;
					}
					c+=inc;

					c3=c*c*c;
					abc3=ab3+c3;
				#endif

				if(abc3!=tdtarg3){continue;}
				else{
					found[tdid]++;
					string write_out =
						ui128tos(tdtarg3)
						+ " = "
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
		for(int i=0; (config_string[i]!='?') && (i<config_string.size()) ;i++){ //word before '='
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
		cout << "#progress_file=" << ui128tos(start) << "\n";
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

