namespace Cinema::VideoManager {
    
    void DownloadCurrentVideo();

    std::string GetCurrentVideoPath();

    bool GetShouldCreateScreen();

    void DidSelectLevel(bool isCustom, std::string levelPath = "");
}