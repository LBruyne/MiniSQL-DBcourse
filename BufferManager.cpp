/*
 * File name: BufferManager.cpp
 * Author: Xuanming, Liu
 * Latest revised: 2020.05.31 
 * Description: 
 * Implementation for BufferManagement.
**/
#include "BufferManager.h"

Page BufferManager::cachePages[CACHE_CAPACITY];
bool BufferManager::pined[CACHE_CAPACITY];
bool BufferManager::isDirty[CACHE_CAPACITY];
int  BufferManager::lruCounter[CACHE_CAPACITY];

map<string, FileHandle> BufferManager::tableFileHandles;
map<pair<string, string>, FileHandle> BufferManager::indexFileHandles;

const string BufferManager::recordFilesDirectory = "RecordFiles";
const string BufferManager::indexFilesDirectory = "IndexFiles";

bool BufferManager::readPage( Page& page )
{
    // TODO: try catch
	assert(page.pageIndex == PageType::Undefined)
    int pageIndex = findPageInCache(page);
    if( pageIndex != PAGE_NOTFOUND ) {
        // read directly from cache.
        page = cachePages[pageIndex];
        lruCounterAddExceptCurrent(pageIndex);
        return true;
    }
    else {
        forceReadPageFromFile(page);
        pageIndex = getUnpinedBiggestPageFromCache();
        if (pageIndex == PAGE_NOTFOUND) {
            return true;
        } else {
            if (isDirty[pageIndex]) {
                isDirty[pageIndex] = false;
                // If it's dirty page, write it again to file
                forceWritePageToFile(cachePages[pageIndex]);
            }
            // use the new page to replace the page
            cachePages[pageIndex] = page;
            lruCounterAddExceptCurrent(pageIndex);
            return true;
        }        
    }
}

bool BufferManager::writePage( Page& page )
{
    assert(page.pageType != PageType::Undefined);
    int pageIndex = findPageInCache(page);
    if ( pageIndex != PAGE_NOTFOUND ) {
        cachePages[pageIndex] = page;
        isDirty[pageIndex] = true;
        lruCounterAddExceptCurrent(pageIndex);
        return true;
    }
    else {
        forceWritePageToFile(page);// May cause bug
        pageIndex = getUnpinedBiggestPageFromCache();
        if (pageIndex == -1) {
            return true;
        } else {
            if (isDirty[pageIndex]) {
                isDirty[pageIndex] = false;
                forceWritePageToFile(cachePages[pageIndex]);
            }
            cachePages[pageIndex] = page;
            lruCounterAddExceptCurrent(pageIndex);
            return true;
        }
    }
    return false;
}

PageIndex BufferManager::findPageInCache( Page& page )
{
    int retIndex = PAGE_NOTFOUND;
    for (int i = 0; i < CACHE_CAPACITY; i++)
        if (cachePages[i] == page) {
            retIndex = i;
            break;
        }
    return retIndex;
}

void BufferManager::lruCounterAddExceptCurrent( int index )
{
    for( int i = 0; i < CACHE_CAPACITY; i++ )
        lruCounter[i]++;
    // newly used, set as 0. Least recently used algorithm.
    lruCounter[index] = 0;
}

bool BufferManager::forceReadPageFromFile( Page& page )
{
    getPageFile(page);
    lseek(page.fileHandle, page.pageIndex * PAGE_SIZE, SEEK_SET);
    return read(page.fileHandle, page.pageData, PAGE_SIZE) != -1;
}

bool BufferManager::forceWritePageToFile( Page& page )
{
    getPageFile(page);
    lseek(page.fileHandle, page.pageIndex * PAGE_SIZE, SEEK_SET);
    return write(page.fileHandle, page.pageData, PAGE_SIZE) != -1;
}

PageIndex BufferManager::getUnpinedBiggestPageFromCache()
{
    int retIndex = PAGE_NOTFOUND;
    int bigSaver = -1;
    for (int i = 0; i < CACHE_CAPACITY; i++) {
        if ((!pined[i]) && (lruCounter[i] > bigSaver)) {
            retIndex = i;
            bigSaver = lruCounter[i];
        }
    }
    return retIndex;
}

void BufferManager::getPageFile( Page& page )
{
    switch (page.pageType) {
        case PageType::IndexPage: {
            if (tableFileHandles.find(page.tableName) == tableFileHandles.end())
                // If not found in map
                assert(openTableFile(page.tableName) == true);
            page.fileHandle = tableFileHandles[page.tableName];
        }
            break;
        case PageType::RecordPage: {

        }
            break;
        default:
            break;
    }
}

bool BufferManager::openTableFile(string tableName)
{
    string filePath = recordFilesDirectory + "/" + tableName + ".db";
    if (tableFileHandles.find(tableName) == tableFileHandles.end()) {
        int fileHandle = open(filePath.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if (fileHandle < 0) return false;
        tableFileHandles[tableName] = fileHandle;
        makeTwoPages(fileHandle);
        return true;
    }
    cout << "File " << filePath << " " << "already opened" << endl;
    return false;   
}



