#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Surface.h"
#include "cinder/ip/Resize.h"
#include "cinder/Thread.h"
#include "cinder/Xml.h"

#include "FileUtilities.h"

typedef std::shared_ptr<std::thread> ThreadRef;

static const int    MAX_NUM_PROGRESS_CIRCLES = 5; // MAXIMUM number of circles for the loading bar

using namespace ci;
using namespace ci::app;
using namespace std;

class ThumbnailManagerApp : public App {
public:
    ~ThumbnailManagerApp();
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void update() override;
    void draw() override;
    
    void deleteThumbnails(const std::string& aMainDir);
    void makeThumbnails(const std::string& aMainDir);
    
    ThreadRef           mThread; // for taking everything to a thread
    int                 mNumOfProgressCircles; // how many circles for the loading bar
    std::string         mMainDirectoryPath, mProcessMethod; // arguments can be from process, mProcessMethod = DELETE || MAKE
    int                 mMaxResizeWidth;// again can be from process, the maximum desired width of the thumbnail
};

void ThumbnailManagerApp::setup()
{
//    mMainDirectoryPath = "/Users/hesamohadi/Desktop/iPhotowall/";
//    mProcessMethod = "DELETE";
//    mMaxResizeWidth = 200;
    
    /* Uncomment the lines above and comment the lines below to disable settings via commandline arguments */
    
    mMainDirectoryPath = getCommandLineArgs()[0];
    mProcessMethod = getCommandLineArgs()[1];
    mMaxResizeWidth = std::stoi(getCommandLineArgs()[2]);
    
    mNumOfProgressCircles =1;
    
    //is it a MAKE process or a DELETE process
    if (mProcessMethod == "MAKE"){
        mThread = ThreadRef( new thread( bind( &ThumbnailManagerApp::makeThumbnails , this, mMainDirectoryPath) ) );
    }
    else if (mProcessMethod == "DELETE"){
        mThread = ThreadRef( new thread( bind( &ThumbnailManagerApp::deleteThumbnails , this, mMainDirectoryPath) ) );
    }
    //quit();
}

void ThumbnailManagerApp::mouseDown( MouseEvent event )
{
}

void ThumbnailManagerApp::makeThumbnails(const std::string &aMainDir)
{
    ci::ThreadSetup threadSetup;
    //parse the folder and generate PhotoInfos
    vector<PhotoInfo> allPhotosInfo = recursiveSearchDirectoryForPhotos(fs::path(aMainDir));
    XmlTree photosXmlTree;
    photosXmlTree.setTag("photos");
    for (auto iter = allPhotosInfo.begin(); iter != allPhotosInfo.end(); ++iter){
        PhotoInfo photoInfo = *iter;
        Surface photo = loadImage(fs::path(photoInfo.mFullPath));
        photoInfo.mResolution = ivec2(photo.getWidth(), photo.getHeight());
        //calculates the the height of the thumbnail based on the maximum resize widths
        ivec2 resizeSize = ivec2(mMaxResizeWidth, mMaxResizeWidth/ photo.getAspectRatio());
        Surface photoResized = ip::resizeCopy(photo, photo.getBounds(), resizeSize);
        std::string photoResizedPath = photoInfo.mDirectoryPath;
        photoResizedPath.append(photoInfo.mFileName).append("_thumbnail").append(photoInfo.mFileExtension);
        writeImage(fs::path(photoResizedPath), photoResized);
        
        //XML Making
        XmlTree photoXmlTree;
        photoXmlTree.setTag("photo");
        photoXmlTree.setAttribute("type", "original");
        photoXmlTree.setAttribute("full_path", photoInfo.mFullPath);
        photoXmlTree.setAttribute("directory_path", photoInfo.mDirectoryPath);
        photoXmlTree.setAttribute("file_name", photoInfo.mFileName);
        photoXmlTree.setAttribute("file_extension", photoInfo.mFileExtension);
        photoXmlTree.setAttribute("resolution_x", photoInfo.mResolution.x);
        photoXmlTree.setAttribute("resolution_y", photoInfo.mResolution.y);
        photosXmlTree.push_back(photoXmlTree);
        //_thumbnal XML making
        XmlTree photoThumbnailXmlTree;
        photoThumbnailXmlTree.setTag("photo");
        photoThumbnailXmlTree.setAttribute("type", "thumbnail");
        photoThumbnailXmlTree.setAttribute("full_path", photoResizedPath);
        photoThumbnailXmlTree.setAttribute("directory_path", photoInfo.mDirectoryPath);
        photoThumbnailXmlTree.setAttribute("file_name", photoInfo.mFileName.append("_thumbnail"));
        photoThumbnailXmlTree.setAttribute("file_extension", photoInfo.mFileExtension);
        photoThumbnailXmlTree.setAttribute("resolution_x", resizeSize.x);
        photoThumbnailXmlTree.setAttribute("resolution_y", resizeSize.y);
        photosXmlTree.push_back(photoThumbnailXmlTree);
    }
    std::string mainDir = aMainDir;
    photosXmlTree.write(writeFile(mainDir.append("/photoInfo.xml")));
    quit();
}

void ThumbnailManagerApp::deleteThumbnails(const std::string &aMainDir)
{
    ci::ThreadSetup threadSetup;
    //parse the folder and generate PhotoInfos
    vector<PhotoInfo> allPhotosInfo = recursiveSearchDirectoryForPhotos(fs::path(aMainDir));
    for (auto iter = allPhotosInfo.begin(); iter != allPhotosInfo.end(); ++iter){
        PhotoInfo photoInfo = *iter;
        // where is "_thumbnail" in the filename?
        int cutPos = photoInfo.mFileName.find("_thumbnail");
        //checks if "thumbnail" is really there
        if (cutPos != std::string::npos){
            //checks if "_thumbnail" is the last word
            if ( photoInfo.mFileName.substr(cutPos) == "_thumbnail" ){
                ci::fs::remove(ci::fs::path(photoInfo.mFullPath));
            }
        }
        
    }
    //checks for xml and deletes it
    std::string mainDir = aMainDir;
    mainDir = mainDir.append("/photoInfo.xml");
    if (ci::fs::exists(ci::fs::path(mainDir))) ci::fs::remove(ci::fs::path(mainDir));
    //adios
    quit();
}

void ThumbnailManagerApp::update()
{
    if ( getElapsedFrames() % 60 == 0){
        mNumOfProgressCircles ++;
        mNumOfProgressCircles %= MAX_NUM_PROGRESS_CIRCLES;
        if (mNumOfProgressCircles == 0) mNumOfProgressCircles = 1;
    }
}

void ThumbnailManagerApp::draw()
{
    gl::clear(ColorAf(0.0, 0.0, 0.0));
    gl::color(1.0, 1.0, 1.0);
    if (mProcessMethod == "MAKE"){
        gl::drawStringCentered("MAKING THUMBNAILS", vec2(getWindow()->getWidth()/2.0, getWindow()->getHeight()/2.0));
    }
    else if (mProcessMethod == "DELETE"){
        gl::drawStringCentered("DELETING THUMBNAILS", vec2(getWindow()->getWidth()/2.0, getWindow()->getHeight()/2.0));
        
    }
    for (int i=0; i<mNumOfProgressCircles; i++){
        float posX = getWindow()->getWidth() / MAX_NUM_PROGRESS_CIRCLES;
        float posY = getWindow()->getHeight() / 3.0 * 2.0;
        gl::color(ColorAf(1.0, 0.0, 0.0));
        gl::drawSolidCircle(vec2(posX * (i+1), posY), posX/4.0);
    }
    
}

ThumbnailManagerApp::~ThumbnailManagerApp()
{
    mThread->join();
}

CINDER_APP( ThumbnailManagerApp, RendererGl(RendererGl :: Options (). msaa ( 16 )), []( App::Settings* settings )
{
    ivec2 windowPos, windowSize;
    windowSize = ivec2(200, 200);
    windowPos = ivec2(Display::getDisplays()[0]->getWidth()/2.0 - windowSize.x/2.0, Display::getDisplays()[0]->getHeight()/2.0 - windowSize.y/2.0);
    
    settings->setBorderless();
    settings->setResizable(false);
    settings->setWindowSize(windowSize);
    settings->setWindowPos(windowPos);
    //settings->setFrameRate(60.0);
    //settings->disableFrameRate();
    settings->setAlwaysOnTop();
    
} )
