
//
//  main.cpp
//  OS
//
//  Created by Mohd Shoaib Rayeen on 10/02/19.
//  Copyright © 2019 Shoaib Rayeen. All rights reserved.
//
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <condition_variable>
#include <thread>
#include <mutex>
#include <chrono>
using namespace std;

struct Node {
    /*
     Structure of Buffer
     */
    struct Node* next;
    struct Node* prev;
    struct Node* freeNext;
    struct Node* freePrev;
    int blockNumber;
    int status;
};
/*
 for status ->  LSB Bit No  ||      Status          ||  Decimal Value if bit is set
                    1st     ->  locked/busy         ->          1
                    2nd     ->  unlocked/free       ->          2
                    3rd     ->  delayed-write       ->          4
                    4th     ->  Valid Data          ->          8
 */

/*
    read -> 0
    write -> 1
    read/write -> 1
 */

mutex mtx , mtx2; //Mutex Lock for Thread Synchronisation

struct Node* bufHead[5] , *bufTail[5]; //Buffer Heads

struct Node* init();            //For Initializing a new Buffer
void insertAtFreeList(int);     //Insertion in Free List
void insertAtHashQueue(struct Node* , int); //Insertion in respective Hash Queue
struct Node* deleteFromFreeList();  // Delete From Free List
void removeFromFreeList(struct Node*);  //Remove From Free List
void getblk(int , bool);   //Get block function
struct Node* search(int);   //Search A block in Buffer Cache
void display(); // Display Current Buffer Cache
void freeListEmpty();   //Randomly add some buffer from free list
void brelse(struct Node* , bool);  //release a buffer after completing its work
void insertAtbegining(struct Node*);    //Insertion at begining in the free list
void insertAtEnding(struct Node*);      //Insertion at end in the free list
void diskWrite();       // Asynchronous write to disk
void signals();         // sending signal to all sleeping processes to wake up if required
void waits();           // sending to sleeping state to a process if required
