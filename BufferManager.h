/*
 * File name: BufferManager.h
 * Author: Xuanming, Liu
 * Latest revised: 2020.06.18
 * Description: 
 * Header for BM module.
 * Served as management for buffer.
 * Function:
 * 1. Read data to buffer or write buffer to file system.
 * 2. Implement the buffer replacement algorithm. When the buffer is full, select the appropriate page replacement.
 * 3. Record the status of each page in the buffer, such as whether it is modified.
 * 4. Provide the Pin function of buffer page which means locking the buffer page to disallow replacement.
**/

#ifndef _MINISQL_BUFFERMANAGER_H_
#define _MINISQL_BUFFERMANAGER_H_

#include "Page.h"
#include "global.h"
using namespace std;

class BufferManager{
private:
    static int lruCounter[CACHE_CAPACITY];
    static Page cachePages[CACHE_CAPACITY]; 
    static bool pined[CACHE_CAPACITY];
    static bool isDirty[CACHE_CAPACITY];

public:
    BufferManager(){
        memset(lruCounter, 0, CACHE_CAPACITY);
        memset(pined, false, CACHE_CAPACITY);
        memset(isDirty, false, CACHE_CAPACITY);
    }

    ~BufferManager(){
        for( int i = 0; i < CACHE_CAPACITY; i++) 
            forceWritePageToFile (cachePages[i]);
    }

    bool readPage( Page& );
    bool writePage( Page& );
    Page& recordManagerGetBlankPage();
    bool pinPage( Page& );
    bool unpinPage( Page& );
    PageIndex findPageInCache(Page&);
private:
    bool forceReadPageFromFile( Page& );
    bool forceWritePageToFile( Page& );
    void lruCounterAddExceptCurrent( int );
    PageIndex getUnpinedBiggestPageFromCache();

};

#endif