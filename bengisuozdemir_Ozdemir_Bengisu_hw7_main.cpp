#include <iostream>
#include <string>
#include <thread>
#include "bengisuozdemir_Ozdemir_Bengisu_hw7_DynIntQueue.h"
#include <random>
#include <time.h>
#include <iomanip>        // for put_time      
#include <chrono>         // for chrono::system_clock
#include <ctime>          // for time_t, tm, localtime, mktime
#include <mutex>
using namespace std;

HW7DynIntQueue filling_queue; //the queue for filling operations
HW7DynIntQueue packaging_queue; //the queue for packaging operations
int total_box_count; //the global variable which stores the total box count
int min_production, max_production, min_filling_time, max_filling_time, min_packaging_time, max_packaging_time; //to be inputted boarders of production/filling/packaging time
mutex producingMutex, coutMutex; //the mutexes for producing and cout operations
mutex fillingMutex; //mutex for filling operations
mutex packingMutex; //mutex for packing operations
int filling_counter = 0; //the global variable to store number of filled boxes
int packing_counter = 0; //the global variable to store number of packaged boxes

int random_range(const int & min, const int & max) 
{
 static mt19937 generator(time(0));
 uniform_int_distribution<int> distribution(min, max);
 return distribution(generator);
}void productionLine(void){	time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now()); 
	struct tm *ptm = new struct tm; 
		for(int i = 1; i <= total_box_count; i++)
	{
		this_thread::sleep_for(chrono::seconds(random_range(min_production, max_production)));
		producingMutex.lock();
		filling_queue.enqueue(i);
		producingMutex.unlock();
		
		tt = chrono::system_clock::to_time_t(chrono::system_clock::now());
		localtime_s(ptm, &tt); 

		coutMutex.lock();
		cout << "Producer has enqueued a new box " << i << " to filling queue (filling queue size is " << filling_queue.getCurrentSize() << "): " << put_time(ptm,"%X") << endl;
		coutMutex.unlock();
	}}
void fillingLine(int fillingID)
{
	int temp;
	bool fillingFinished = false;
	while(filling_counter < total_box_count)
	{
		
		fillingMutex.lock();
		
		while(filling_queue.isEmpty() && !fillingFinished) 
		{
			fillingMutex.unlock();
			fillingMutex.lock();
			if (filling_counter == total_box_count)
			{
				fillingFinished = true;
			}
		}
		if (!fillingFinished)
		{
			filling_queue.dequeue(temp);
			filling_counter++;
			fillingMutex.unlock();

			coutMutex.lock();
			time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now()); 
			struct tm *ptm = new struct tm; 
			localtime_s(ptm, &tt); 
			cout<< "Filler " << fillingID <<" started filling the box "<< temp << " (filling queue size is " << filling_queue.getCurrentSize() <<"): " << put_time(ptm,"%X") <<endl;
			coutMutex.unlock();

			this_thread::sleep_for(chrono::seconds(random_range(min_filling_time, max_filling_time)));

			coutMutex.lock();
			tt = chrono::system_clock::to_time_t(chrono::system_clock::now()); 
			localtime_s(ptm, &tt); 
			cout<< "Filler " << fillingID <<" finished filling the box "<< temp << " (filling queue size is " << filling_queue.getCurrentSize() <<"): " << put_time(ptm,"%X") <<endl;
			coutMutex.unlock();
		
			this_thread::sleep_for(chrono::seconds(random_range(min_filling_time, max_filling_time)));

			fillingMutex.lock();
			packaging_queue.enqueue(temp);
			fillingMutex.unlock();

			coutMutex.lock();
			tt = chrono::system_clock::to_time_t(chrono::system_clock::now()); 
			localtime_s(ptm, &tt); 
			cout<< "Filler " << fillingID <<" put box "<< temp << " into packaging queue (packaging queue size is " << packaging_queue.getCurrentSize() <<"): " << put_time(ptm,"%X") <<endl;
			coutMutex.unlock();
		}
		else
		{
			fillingMutex.unlock();
		}
	}
}void packagingLine(int packingID){	int temp_item = 0;	bool packingFinished = false;	while(packing_counter < total_box_count)	{		packingMutex.lock();		while(packaging_queue.isEmpty() && !packingFinished)		{			packingMutex.unlock();
			packingMutex.lock();
			if (packing_counter == total_box_count)
			{
				packingFinished = true;
			}		}		if(!packingFinished)		{			packaging_queue.dequeue(temp_item);			packing_counter++;			packingMutex.unlock();			coutMutex.lock();
			time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now()); 
			struct tm *ptm = new struct tm; 
			localtime_s(ptm, &tt); 
			cout<< "Packager " << packingID <<" started packaging the box  "<< temp_item << " (packaging queue size is " << packaging_queue.getCurrentSize() <<"): " << put_time(ptm,"%X") <<endl;
			coutMutex.unlock();			this_thread::sleep_for(chrono::seconds(random_range(min_packaging_time, max_packaging_time)));			coutMutex.lock();
			tt = chrono::system_clock::to_time_t(chrono::system_clock::now()); 
			localtime_s(ptm, &tt); 
			cout<< "Filler " << packingID <<"  finished packaging the box "<< temp_item << " (packaging queue size is " << packaging_queue.getCurrentSize() <<"): " << put_time(ptm,"%X") <<endl;
			coutMutex.unlock();		}		else		{			packingMutex.unlock();		}	}}
int main()
{
	cout << "Please enter the total number of items: ";
	cin>> total_box_count;
	cout << "Please enter the min-max waiting time range of producer:" << endl << "Min: ";
	cin >>min_production;
	cout << "Max: ";
	cin >>max_production;
	cout << "Please enter the min-max waiting time range of filler workers:" << endl << "Min: ";
	cin >>min_filling_time;
	cout << "Max: ";
	cin >>max_filling_time;
	cout << "Please enter the min-max waiting time range of packager workers:" << endl << "Min: ";
	cin >>min_packaging_time;
	cout << "Max: ";
	cin >>max_packaging_time;
	//beginning: code taken from ProducerConsumer.cpp
	time_t tt = chrono::system_clock::to_time_t(chrono::system_clock::now()); 
	struct tm *ptm = new struct tm; 
	localtime_s(ptm, &tt);
	//ending: code take from ProducerConsumer.cpp
	cout << "Simulation starts " << put_time(ptm,"%X") << endl;
	thread producer(productionLine);
	thread filler1(fillingLine, 1);
	thread filler2(fillingLine, 2);
	thread packager1(packagingLine, 1);
	thread packager2(packagingLine, 2);
	producer.join();
	filler1.join();
	filler2.join();
	packager1.join();
	packager2.join();
	tt = chrono::system_clock::to_time_t(chrono::system_clock::now()); 
	localtime_s(ptm, &tt); 
	cout << "End of the simulation ends: " << put_time(ptm,"%X") <<endl;
	delete ptm;
	return 0;
}