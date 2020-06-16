/*
 * File name: Page.h
 * Author: Xuanming, Liu
 * Latest revised: 2020.05.31 
 * Description: 
 * Header for Page class.
 * One Page is a basic unit for reading and writing data.
**/

#ifndef _MINISQL_PAGE_H_
#define _MINISQL_PAGE_H_

#include "global.h"
using namespace std;

class Page{
public:
    string tableName, attributeName;
    PageType pageType;
    PageIndex pageIndex;
    FileHandle fileHandle;
    char pageData[PAGE_SIZE];

    Page() {
        tableName = "";
        attributeName = "";
        pageType = PageType::Undefined;
        pageIndex = -1;
        memset(pageData, '@', PAGE_SIZE);
    }

    ~Page(){
    }

    // compare two page.
    bool operator==(const Page& page) {
        return (this->attributeName == page.attributeName) && (this->tableName == page.tableName) && (this->pageType == page.pageType) && (this->pageIndex == page.pageIndex);
    }
};

#endif