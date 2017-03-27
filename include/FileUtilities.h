//
//  FileUtilities.h
//  iPhotoWall
//
//  Created by Hesam Ohadi on 26/10/15.
//
//

#ifndef __iPhotoWall__FileUtilities__
#define __iPhotoWall__FileUtilities__

#include <stdio.h>
#include "cinder/Filesystem.h"
#include <boost/filesystem.hpp>
#include "cinder/Utilities.h"

//The info for each photo
struct PhotoInfo{
    std::string         mFullPath;
    std::string         mDirectoryPath;
    std::string         mFileName;
    std::string         mFileExtension; //includes dot --> ".png"
    std::string         mAssociatedText; // a text file with same name as the photo
    ci::ivec2           mResolution; // we fill this out after we read the image
};

// search the directory and get the info for any photos there
inline std::vector<PhotoInfo>      searchDirectoryForPhotos(const ci::fs::path& aDirectoryPath)
{
    std::vector<PhotoInfo> result;
    if (!ci::fs::is_directory(aDirectoryPath)) return result;
    
    ci::fs::directory_iterator dirIter(aDirectoryPath), eod;
    for ( ; dirIter != eod; ++dirIter){
        ci::fs::path file = *dirIter;
        
        
        std::string fullPath, directoryPath, fileName, fileExtension;
        fullPath = file.string();
        directoryPath = file.parent_path().string();
        // checks if the path supplied by the user ends with a '/'
        if (directoryPath.back() != '/') directoryPath = directoryPath.append("/");
        fileName = file.stem().string();
        fileExtension = file.extension().string();
        if (fileExtension == ".png" || fileExtension == ".jpg" || fileExtension == ".bmp" ){
            
            PhotoInfo photoInfo;
            photoInfo.mFullPath = fullPath;
            photoInfo.mDirectoryPath = directoryPath;
            photoInfo.mFileName = fileName;
            photoInfo.mFileExtension = fileExtension;
            // checks if there is a text file associated with the photo
            ci::fs::path textFilePath = ci::fs::path(directoryPath.append(fileName).append(".txt"));
            if(ci::fs::exists(textFilePath)) {
                photoInfo.mAssociatedText = ci::loadString(ci::loadFile(textFilePath));
            }
            result.push_back(photoInfo);
        }
    }
    return result;
}

// search the directory recursively and get the info for any photos there
inline std::vector<PhotoInfo>      recursiveSearchDirectoryForPhotos(const ci::fs::path& aDirectoryPath)
{
    std::vector<PhotoInfo> result;
    if (!ci::fs::is_directory(aDirectoryPath)) return result;
    
    ci::fs::recursive_directory_iterator dirIter(aDirectoryPath), eod;
    for ( ; dirIter != eod; ++dirIter){
        ci::fs::path file = *dirIter;
        
        
        std::string fullPath, directoryPath, fileName, fileExtension;
        fullPath = file.string();
        directoryPath = file.parent_path().string();
        // checks if the path supplied by the user ends with a '/'
        if (directoryPath.back() != '/') directoryPath = directoryPath.append("/");         fileName = file.stem().string();
        fileExtension = file.extension().string();
        if (fileExtension == ".png" || fileExtension == ".jpg" || fileExtension == ".bmp"){
            
            PhotoInfo photoInfo;
            photoInfo.mFullPath = fullPath;
            photoInfo.mDirectoryPath = directoryPath;
            photoInfo.mFileName = fileName;
            photoInfo.mFileExtension = fileExtension;
            // checks if there is a text file associated with the photo
            ci::fs::path textFilePath = ci::fs::path(directoryPath.append(fileName).append(".txt"));
            if(ci::fs::exists(textFilePath)) {
                photoInfo.mAssociatedText = ci::loadString(ci::loadFile(textFilePath));
            }
            result.push_back(photoInfo);
        }
    }
    return result;
}

#endif /* defined(__iPhotoWall__FileUtilities__) */
