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

std::mutex tasksMutex;
std::condition_variable tasksCV;
bool threadready = true;

//Mutex for synchronization
std::vector<std::vector<vqint3>> sharedResults;
using MutexVector = std::vector<std::unique_ptr<std::mutex>>;

// Define a condition variable and a flag to indicate an update is needed
std::condition_variable bgtasksCV;
bool updateBgtasks = false;

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

void bgtasks(
	string resultsFile,
	uint64_t tdtarg, 
	MutexVector& resultsMutex,
	uint64_t bguprate
){
	thread inpthread = thread(readinput);
	std::vector<std::vector<vqint3>> buffer(sharedResults.size());

	ofstream tdfile;
	tdfile.open(resultsFile, ios::out | ios::app);

	uint64_t loopinc=0;
	while(threads_active==1){

		// Check if an update is needed
		std::unique_lock<std::mutex> updateLock(tasksMutex);
		bgtasksCV.wait_for(
			updateLock, 
			std::chrono::milliseconds(10), 
			[&]() {return updateBgtasks;}
		);
		if(updateBgtasks){
			tdfile << "#a[0]: " << bguprate*(loopinc++) << "\n" << flush;
		}
		updateBgtasks = false;  // Reset the update flag

		//check input
		if (input == "q" || input == "quit") {
			std::cout << "#User exit\n";
			std::cout << "#Closing threads\n";

			unique_lock<mutex> lck(tasksMutex);
			threadready = 0;
			update_rate = 1;
			for (uint i = 0; i < active_thread.size(); i++) {
				active_thread[i] = 0;
			}
			threadready = 1;
			lck.unlock();
			tasksCV.notify_all();
			threads_active = 0;
		}

		//check if there's active threads
		bool noActiveThreads=1;
		for (const auto& isActive : active_thread) {
			if(isActive){noActiveThreads=0;};
		}
		if(noActiveThreads==1){
			bool unwrittenData = 0;
			//check if sharedResults has unwritten data
			for (uint i = 0; i < active_thread.size(); ++i) {
				std::lock_guard<std::mutex> lock(*resultsMutex[i]);
				if(sharedResults[i].size()){
					unwrittenData=1;
				};
			}
			//exit if no unwritten data and no active threads
			if(!unwrittenData){
				cout << "#Threads finished\n";
				cout << "#Press enter\n";
				threads_active=0;
			}
		}
		
		//Copy shared thread results to buffer. per thread mutex scope
		for (uint i = 0; i < active_thread.size(); ++i) {
			std::lock_guard<std::mutex> lock(*resultsMutex[i]);
			uint64_t copySize = sharedResults[i].size();
			if(copySize==0){continue;}
			buffer[i] = sharedResults[i];
			sharedResults[i].clear();
		}

		//write from buffer
		for (const auto& resultVec : buffer) {
			if(resultVec.size()==0){continue;}
			for (uint i=0; i< resultVec.size(); i++) {
				string write_out =
					to_string(tdtarg) + "^3"
					+ " = "
					+ to_string((uint64_t)(resultVec[i].x)) + "^3+"
					+ to_string((uint64_t)(resultVec[i].y)) + "^3+"
					+ to_string((uint64_t)(resultVec[i].z)) + "^3"
					+ "\n"
				;
				tdfile << write_out << flush;
				found[i]++;
			}
		}

		for(uint i=0;i<active_thread.size();i++){ buffer[i].clear();	} //clear buffer

	}
	
	buffer.clear();
	tdfile.close();
	inpthread.detach(); //will not free memory
	return;
}



//nmcbrt follows more closely cubesum-cuda implementation

//THREAD FUNCTIONS
//(for any natural a,b a>b tdtarg3=a^3+b^3+c^3 find c by quadratic convergence)
void newtoncbrt(
	const uint index, 
	const uint stride, 
	const  __uint64_t aofs, 
	const  __uint128_t tdtarg, 
	const  __uint128_t tdtarg3, 
	__uint64_t tdmaxa, 
	std::mutex& resultsMutex
 ){

	__uint64_t a,b,c;
	__uint128_t ai3,bc3,ctarg;
	__uint128_t c0=tdtarg-1;
	__uint64_t updateinc = 0;
	//search a,b. Index starts from 1
	for (a = index+aofs+1; a<tdmaxa+1; a += stride){

		updateinc++;
		if( updateinc>update_rate ){ //update periodically
			updateinc=0;
			if(index==0){ //1st thread checks this only
				updateBgtasks = true;
   				bgtasksCV.notify_one();  // Notify the bgtasks() thread
			}

			//make sure writing is not in progress
  			unique_lock<mutex> lck(tasksMutex);
			while (!threadready) tasksCV.wait(lck);
				bool isactive = active_thread[index];

			//check if closing threads
			if(isactive==0){
				last_compute[index]=a-stride;
				return;
			}
		}

		ai3=(__uint128_t)a*a*a;
		bc3=tdtarg3-ai3; //b^3+c^3
		c0=tdtarg-1;
	
		ctarg=bc3-ai3;
		do{ //quadratic iterations c for (a^3+(a-1)^3+c^3)^(1/3)
			c = c0;
			c0 = ((__uint128_t)3*c + ctarg/((__uint128_t)c*c))/4; //newton's method
//			c0 = ((3*ctarg)/((2*c*c*c+ctarg))-1)/(2*c)+c*c*c-ctarg; //halley's steps
		}while (c0 < c);
		
		for (b = a+1; b<c; b++) {
			ctarg=bc3-(__uint128_t)b*b*b; //c^3

			//only one newton's method pass is required for each b++.
			c = ((__uint128_t)3*c + ctarg/((__uint128_t)c*c))/4;

			//if ctarg^(1/3) has integer solution
			if ((__uint128_t)c * c * c == ctarg) {
				std::unique_lock<std::mutex> lock(resultsMutex);
				sharedResults[index].push_back(vqint3{a, b, c});
			}
		
		}
	}

	active_thread[index]=0;
	last_compute[index]=a;
	return;
}


//finds target for target^3=A^3+B^3+C^3 	//check set [target,start] 	//time increases proportionately s^2 
int main( int argc, char *argv[] ){

	//BOF config
	
	uint tc=1; //thread count
	__uint128_t start=0; //start 
	__uint128_t target=1000; //target

	string thread_file_suffix = "x";
	string work_directory = "./";
	string config = work_directory+"prime.cfg";	//config file
	string progress_file = work_directory+"lastSolve.txt";
	string results_file = work_directory+"sharedResults.txt";
	bool clear_file=0;
	update_rate=256;
	uint64_t bguprate = 256;
	
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
					start=stol(value); 
					if(start<0){start=0;}
				}else if(	word=="target" ){
					target=abs(stol(value)); 
					if(target<=1){target=2;}
				}else if(	word=="progress_file"){
					progress_file=value;
				}else if(	word=="update_rate" ){
					bguprate=stol(value);
					update_rate=bguprate; 
					if(update_rate<=0){update_rate=target;bguprate=target;}
				}else if(	word=="work_directory" ){
					work_directory=value;
				}else if(	word=="clear_file" ){
					clear_file=stol(value);
				}else if(	word=="results_file" ){
					if(value==""){cout << "#Invalid results_file!\n";}
					value=work_directory+"sharedResults.txt";
					results_file=value;
				}
				cout << "#" << word << "=" << value << "\n";
				word="";
			}
		}
	}

	if(clear_file==1){
		ofstream resfil;
		resfil.open(work_directory+results_file, ios::out);
		resfil << "";
		resfil.close();
	}
	
	if(progress_file!=""){//progress file
		ifstream in_progress_file;
		in_progress_file.open(work_directory+progress_file, ios::in);

		stringstream tmp;
		tmp << in_progress_file.rdbuf();
		in_progress_file.close();

		string progress_string = tmp.str();

		string word="";
		for(uint i=0;progress_string[i];i++){ //word before '='
			if(progress_string[i]!='\n'){
				word+=progress_string[i];
			}
		}
		if(word!=""){
			start=stoi(word);
		}
		cout << "#progress_file=" << to_string((uint64_t)start) << "\n";
	}
	//EOF config
	

	//calculate A range offsets per thread
	static const __uint128_t aofs=start; 	//A start

	static const __uint128_t target3=target*target*target;

	//BOF tasks
	thread *task = new thread[tc];
	thread bgthread;

	__uint128_t maxa=target*(__float128)0.693361274350634659846548402128973976+1; //max A = t * 3^(2/3)/3

	MutexVector resultsMutex(tc); // Create vector of resultsMutex
	
	for(uint i=0;i<tc;i++){  //spawn threads
		active_thread.push_back(1);
		resultsMutex[i] = std::make_unique<std::mutex>();
		sharedResults.push_back(std::vector<vqint3>());
		found.push_back(0);
		last_compute.push_back(0);
		task[i] = thread(
			newtoncbrt,			//thread function
			i,					//thread index
			tc,					//thread count and A stride increment
			aofs,				//thread start
			target,				//N
			target3,			//N^3
			maxa,				//max A
			std::ref(*resultsMutex[i])
		);
	}

	//background tasks
	bgthread = thread(
		bgtasks,
		work_directory+results_file,
		target,
		std::ref(resultsMutex),
		bguprate
	);

	//synchronize threads
	for(uint i=0;i<tc;i++){
		task[i].join();
	}
	bgthread.join();

  	delete [] task;
	//EOF tasks

	__uint64_t foundsum=0;
	__uint64_t lastsolve=-1;
	for(uint i=0;i<tc;i++){
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

	return 0;
}

