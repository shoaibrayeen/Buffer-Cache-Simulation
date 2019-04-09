//
//  getblk.cpp
//  OS
//
//  Created by Mohd Shoaib Rayeen on 10/02/19.
//  Copyright © 2019 Shoaib Rayeen. All rights reserved.
//

#include "getblk.h"

void display() {
    printf("\n--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("\tHeaders\t\t|\tBlock Number in Respective Hash Queue Headers\n");
    printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    for ( int i = 0; i < 5; i++ ) {
        if( i != 4) {
            printf("\t%d\t\t|\t" , i);
            if( bufHead[i] != NULL ) {
                struct Node* temp = bufHead[i];
                while( temp != bufTail[i] ) {
                    printf( "%d" , temp->blockNumber);
                    if( (temp->status & 4) == 4 ) {
                        printf("(Delayed-write)");
                    }
                    printf("\t");
                    temp = temp->next;
                }
                printf( "%d" , temp->blockNumber);
            }
        }
        else {
            printf("\tFree\t\t|\t");
            if( bufHead[4] != NULL ) {
                struct Node* temp = bufHead[i];
                while( temp != bufTail[i] ) {
                    printf( "%d" , temp->blockNumber);
                    if( (temp->status & 4) == 4 ) {
                        printf("(Delayed-write)");
                    }
                    printf("\t");
                    temp = temp->freeNext;
                }
                printf( "%d" , temp->blockNumber);
            }
        }
        printf("\n--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    }
}


struct Node* init() {
    struct Node* temp = (struct Node*) malloc(sizeof(struct Node));
    temp->next = NULL;
    temp->prev = NULL;
    temp->freeNext = NULL;
    temp->freePrev = NULL;
    temp->status = 2;
    return temp;
}


void insertAtFreeList(int value) {
    struct Node* temp = init();
    temp->blockNumber = value;
    if( bufHead[4] == NULL ) {
        bufHead[4] = temp;
        bufTail[4] = temp;
        bufHead[4]->freeNext = bufHead[4];
        bufHead[4]->freePrev = bufHead[4];
    }
    else {
        bufTail[4]->freeNext = temp;
        temp->freePrev = bufTail[4];
        temp->freeNext = bufHead[4];
        bufHead[4]->freePrev = temp;
        bufTail[4] = temp;
    }
}

void insertAtHashQueue(struct Node* temp , int value) {
    temp->blockNumber = value;
    int index = value % 4;
    if( bufHead[index] == NULL ) {
        bufHead[index] = temp;
        bufTail[index] = temp;
        bufHead[index]->next = bufHead[index];
        bufHead[index]->prev = bufHead[index];
    }
    else {
        bufTail[index]->next = temp;
        temp->prev = bufTail[index];
        temp->next = bufHead[index];
        bufHead[index]->prev = temp;
        bufTail[index] = temp;
    }
}


void removeFromFreeList(struct Node* temp ) {
    if( temp == bufHead[4] && bufHead[4] == bufTail[4] ) {
        bufHead[4] = NULL;
        bufTail[4] = NULL;
    }
    else if (temp == bufHead[4] ) {
        bufHead[4] = bufHead[4]->freeNext;
        bufTail[4]->freeNext = bufHead[4];
        bufHead[4]->freePrev = bufTail[4];
    }
    else if ( temp == bufTail[4] ) {
        bufTail[4] = bufTail[4]->freePrev;
        bufTail[4]->freeNext = bufHead[4];
        bufHead[4]->freePrev = bufTail[4];
    }
    else {
        temp->freePrev->freeNext = temp->freeNext;
        temp->freeNext->freePrev = temp->freePrev;
    }
    temp->freeNext = NULL;
    temp->freePrev = NULL;
}


struct Node* deleteFromFreeList() {
    struct Node* temp = bufHead[4];
    if( bufHead[4] == bufTail[4] ) {
        bufHead[4] = NULL;
        bufTail[4] = NULL;
    }
    else {
        bufHead[4] = bufHead[4]->freeNext;
        bufTail[4]->freeNext = bufHead[4];
        bufHead[4]->freePrev = bufTail[4];
    }
    temp->freeNext = NULL;
    temp->freePrev = NULL;
    //Removing Node from Buffer Cache
    int index = temp->blockNumber % 4;
    if ( search(temp->blockNumber) != NULL ) {
        printf("\nReassigning Block\t:\t%d\n", temp->blockNumber);
        if( temp == bufHead[index] && bufHead[index] == bufTail[index] ) {
            bufHead[index] = NULL;
            bufTail[index] = NULL;
        }
        else if (temp == bufHead[index] ) {
            bufHead[index] = bufHead[index]->next;
            bufTail[index]->next = bufHead[index];
            bufHead[index]->prev = bufTail[index];
        }
        else if ( temp == bufTail[index] ) {
            bufTail[index] = bufTail[index]->prev;
            bufTail[index]->next = bufHead[index];
            bufHead[index]->prev = bufTail[index];
        }
        else {
            temp->prev->next = temp->next;
            temp->next->prev = temp->prev;
        }
    }
    return temp;
}

struct Node* search(int tempBlockNumber) {
    int index = tempBlockNumber % 4;
    if ( bufHead[index] != NULL) {
        struct Node* temp = bufHead[index];
        do {
            if( temp->blockNumber == tempBlockNumber) {
                return temp;
            }
            temp = temp->next;
            if (temp->blockNumber == tempBlockNumber) {
                return temp;
            }
        } while(temp != bufTail[index]);
        return NULL;
    }
    return NULL;
}

void freeListEmpty() {
    printf("\nFree List Empty ->Process Id\t:\t%d\n" , this_thread::get_id());
    int index;
    do {
        index = rand() % 4;
    }while( bufHead[index] == NULL);
    mtx.lock();
    struct Node* temp = bufHead[index];
    temp->status = 2;
    insertAtEnding(temp);
    mtx.unlock();
}


void insertAtbegining(struct Node* temp) {
    if( bufHead[4] == NULL && bufTail[4]== NULL ) {
        bufHead[4] = temp;
        bufTail[4] = temp;
        bufHead[4]->freeNext= bufHead[4];
        bufHead[4]->freePrev = bufHead[4];
    }
    else {
        temp->freeNext = bufHead[4];
        bufHead[4]->freePrev = temp;
        temp->freePrev = bufTail[4];
        bufTail[4]->freeNext = temp;
        bufHead[4] = temp;
    }
}

void insertAtEnding(struct Node* temp) {
    if( bufHead[4] == NULL && bufTail[4]== NULL ) {
        bufHead[4] = temp;
        bufTail[4] = temp;
        bufHead[4]->freeNext= bufHead[4];
        bufHead[4]->freePrev = bufHead[4];
    }
    else {
        bufTail[4]->freeNext = temp;
        temp->freePrev = bufTail[4];
        temp->freeNext = bufHead[4];
        bufHead[4]->freePrev = temp;
        bufTail[4] = temp;
    }
}

void brelse(struct Node* temp , bool status) {
    if ( status == false ) {
        temp->status = 2;
        insertAtEnding(temp);
    }
    else {
        temp->status = 4;
        insertAtbegining(temp);
    }
}

void diskWrite() {
    if( bufHead[4]->status != 4) {
        return;
    }
    printf("\nAsynchonous Write ->Process Id\t:\t%d\n" , this_thread::get_id());
    this_thread::sleep_for (std::chrono::seconds(1));
    bufHead[4]->status = 1;
    struct Node* temp = bufHead[4];
    removeFromFreeList(temp);
    display();
    this_thread::sleep_for (std::chrono::seconds(4));
    temp->status = 2;
    insertAtbegining(temp);
}

void lock() {
    mtx.lock();
}

void unlock() {
    mtx.unlock();
}

int i = 0;
condition_variable cv;
mutex cv_m; 
bool flag = false;
void waits() {
    unique_lock<mutex> lk(cv_m);
    cerr << "\nWaiting for buffer to be free \n";
    flag = true;
    cv.wait(lk, []{return i == 1;});
    sleep(3);
    cerr << "\nBuffer is Free. Finished waiting.\n";
}

void signals() {
    this_thread::sleep_for(chrono::seconds(1)); 
    {
        lock_guard<mutex> lk(cv_m);
        cerr << "\nNotifying to Waiting Process\n";
    }
    cv.notify_all();
 
    this_thread::sleep_for(chrono::seconds(1));
    {
        lock_guard<mutex> lk(cv_m);
        i = 1;
        cerr << "\nNotifying to All Waiting Processes\n";
    }
    cv.notify_all();
}
void getblk(int tempBlockNumber , bool status ) {
    while (true) {
        struct Node* temp = search(tempBlockNumber);
        if( temp != NULL ) {
            printf("\nBuffer found in Cache\n");
            sleep(2);
            if(temp->status == 1 ) {
                //scenario 5
                printf("\nBuffer Busy\n");
                waits();
                //this_thread::sleep_for (std::chrono::seconds(3));
                continue;
            }
            //scenario 1
            mtx2.lock();
            temp->status = 1;
            removeFromFreeList(temp);
            printf("\nLocked Buffer\t:\t%d\tBy Process Id\t:\t%d\n" , tempBlockNumber , this_thread::get_id());
            display();
            this_thread::sleep_for (std::chrono::seconds(8));
            brelse(temp , status);
            printf("\nUnlocked Buffer\t:\t%d\tBy Process Id\t:\t%d\n" , tempBlockNumber , this_thread::get_id());
            this_thread::sleep_for (std::chrono::seconds(4));
            if ( flag == true ) {
                signals();
                flag = false;
            }
            mtx2.unlock();
            break;
        }
        else {
            printf("\nBuffer Not found in Cache\n");
            sleep(2);
            if( bufHead[4] == NULL ) {
                printf("\nEmpty Free List\n");
                mtx2.lock();
                thread tid(freeListEmpty);
                tid.join();
                mtx2.unlock();
                //SCENARIO 4
                // any process comes and add some buffer in free list
                
                continue;
            }
            if( bufHead[4]->status == 4) {
                printf("\nAsynchronous Write to Disk Cause of Delayed-write in Free List Head Buffer\n");
                mtx2.lock();
                thread tid(diskWrite);
                tid.join();
                mtx2.unlock();
                //scenario 3
                //asynchronous write -> random process comes and modify buffer status as free
                continue;
            }
            //scenario 2
            //sleep(2);
            printf("\nRemoving 1st Block from Free List\n");
            mtx2.lock();
            temp = deleteFromFreeList();
            temp->status = 1;
            insertAtHashQueue(temp, tempBlockNumber);
            printf("\nLocked Buffer\t:\t%d\tBy Process Id\t:\t%d\n" , tempBlockNumber , this_thread::get_id());
            this_thread::sleep_for (std::chrono::seconds(16));
            temp->status = 2;
            brelse(temp , status);
            mtx2.unlock();
            printf("\nUnlocked Buffer\t:\t%d\tBy Process Id\t:\t%d\n" , tempBlockNumber , this_thread::get_id());
            this_thread::sleep_for (std::chrono::seconds(3));
            if ( flag == true ) {
                signals();
                flag = false;
            }
            break;
        }
    }
}



void putBlockNumber() {
    lock();
    printf("\nProcess Id\t:\t%d\n" , this_thread::get_id());
    int value , rwbit;
    bool flag = true;;
    printf("\nEnter Block Number\t:\t");
    scanf("%d" , &value);
    printf("\nWhy Need Block Number\t:\t%d\n1. Read\n2. Write\n3. Read/Write\nEnter Your Choice\t:\t" , value);
    scanf("%d" , &rwbit);
    if ( rwbit == 1) {
        flag = false;
    }
    unlock();
    getblk(value , flag);
}

void mainLoop() {
    int ch;
    while (true) {
        //lock();       // for Displaying How Different Prorcesses Work
        printf("\n1. Display Current HashQueue");
        printf("\n2. Call getblk()");
        printf("\n3. Exit\n");
        lock();
        printf("\nProcess Id\t:\t%d\n" , this_thread::get_id());
        printf("\nEnter Your Choice\t:\t");
        scanf("%d" , &ch);
        unlock();
        switch (ch) {
            case 1:
                lock();
                display();
                unlock();
                break;
            case 2: putBlockNumber();
                
                break;
            case 3:
                exit(0);
            default: printf("\nInvalid Input\n");
        }
        //unlock();
    }
}


int main() {
    int total;
    printf("\nEnter Total Number of Buffers\t:\t");
    scanf("%d" , &total);
    for(int i = 0; i < total; i++ ) {
        insertAtFreeList(i+1);
    }
    display();
    sleep(2);
    while ( true ) {
        thread t1(mainLoop);
        thread t2(mainLoop);
        thread t3(mainLoop);
        thread t4(mainLoop);
        t1.join();
        t2.join();
        t3.join();
        t4.join();
    }
    return 0;
}
