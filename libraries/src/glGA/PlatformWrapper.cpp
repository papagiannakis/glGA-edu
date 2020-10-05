//
//  PlatformWrapper.cpp
//
//
//  Created by George Papagiannakis.
//  Copyright (c) 2013 UoC & FORTH. All rights reserved.
//

#include "glGA/PlatformWrapper.h"

#if defined (__WINDOWS__MAC__LINUX__)

string getTextureResourcePath(const char* filename){
	return string(filename);
}

string getGLSLResourcePath(const char* filename){
	return string(filename);
}

string getModelResourcePath(const char* filename){
	return string(filename);
}

#elif defined (__IOS__)

#include <CoreFoundation/CoreFoundation.h>

string getTextureResourcePath(const char* filename){
	return getBundlePath(filename);
}

string getGLSLResourcePath(const char* filename){
	return getBundlePath(filename);
}

string getModelResourcePath(const char* filename){
	return getBundlePath(filename);
}

string getFilenameWithoutExtension(const char* filename){
    string str = filename;
    size_t found=str.find_last_of(".");
    return str.substr(0, found);
}

string getExtension(const char* filename){
    string str = filename;
    size_t found=str.find_last_of(".");
    return str.substr(found+1);
}

string getExtensionWithDot(const char* filename){
    string extension =getExtension(filename);
    return string(".") + extension;
}

void getRidOfString(string& full_string, string part_string){
    int index_of_part = full_string.find(part_string);
    if (index_of_part == -1)
        return;
    int size_of_part = part_string.size();
    full_string.replace(index_of_part, size_of_part, "");
    
}

void replaceURLSpaces(string& str){
    string url_space_string = string("%20");
    string correct_space_string = string(" ");
    
    int index_of_url_space = str.find(url_space_string);
    while (index_of_url_space != -1){
        str.replace(index_of_url_space, url_space_string.size(), correct_space_string);
        index_of_url_space = str.find(url_space_string);
    }
    
}

string getBundlePath(const char* filename){
    CFBundleRef main_bundle = CFBundleGetMainBundle();
    
    CFStringRef filename_without_extension = CFStringCreateWithCString(
                                                                       kCFAllocatorDefault,
                                                                       getFilenameWithoutExtension(filename).c_str(),
                                                                       kCFStringEncodingMacRoman
                                                                       );
    
    CFStringRef extenstion = CFStringCreateWithCString(
                                                       kCFAllocatorDefault,
                                                       getExtensionWithDot(filename).c_str(),
                                                       kCFStringEncodingMacRoman
                                                       );
    
    CFURLRef file_url = CFBundleCopyResourceURL(main_bundle, filename_without_extension, extenstion, NULL);
    CFStringRef string_url = CFURLGetString(file_url);
    long length_of_string = CFStringGetLength(string_url);
    //cout << length_of_string << endl;
    
    char* c_string = (char*)malloc(length_of_string);
    CFStringGetCString(string_url, c_string, FILENAME_MAX, kCFStringEncodingUTF8);
    
    //cout << c_string << endl;
    
    string str = string(c_string);
    getRidOfString(str, "file://");
    getRidOfString(str, "localhost");
    replaceURLSpaces(str);
    
    return str;
}

#endif
