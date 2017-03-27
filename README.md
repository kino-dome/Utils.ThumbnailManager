# Utility__ThumbnailManager
The code for a utility app that makes and deletes thumbnails via command line argumnents

The app gets 2 or 3 arguments:
1- Path (string) for the folder to make/delete the thumbnails in. (it checks if the path ends with '/' or not)
2- "MAKE" or "DELETE"
3- Maximum resize width for the thumbnail (the height is calculated by the photo's aspect ratio). This is only relevant is the previous argument is "MAKE"
