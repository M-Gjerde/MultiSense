//
// Created by magnus on 3/1/22.
//

#include "CRLPhysicalCamera.h"


void CRLPhysicalCamera::connect() {
    online = CRLBaseCamera::connect(DEFAULT_CAMERA_IP);

}

void CRLPhysicalCamera::start(std::string string, std::string dataSourceStr) {

    crl::multisense::DataSource source = stringToDataSource(dataSourceStr);
    enabledSources.push_back(source);
    if (source == crl::multisense::Source_Chroma_Rectified_Aux)
        enabledSources.push_back(crl::multisense::Source_Luma_Rectified_Aux);

    // Set mode first
    std::string delimiter = "x";

    size_t pos = 0;
    std::string token;
    std::vector<uint32_t> widthHeightDepth;
    while ((pos = string.find(delimiter)) != std::string::npos) {
        token = string.substr(0, pos);
        widthHeightDepth.push_back(std::stoi(token));
        string.erase(0, pos + delimiter.length());
    }

    this->selectDisparities(widthHeightDepth[2]);
    this->selectResolution(widthHeightDepth[0], widthHeightDepth[1]);
    this->selectFramerate(60);

    // Start stream
    for (auto src: enabledSources) {
        bool status = cameraInterface->startStreams(src);
        printf("Started stream %s status: %d\n", dataSourceToString(src).c_str(), status);
    }


    this->modeChange = true;
}

void CRLPhysicalCamera::stop(std::string dataSourceStr) {
    // Start stream
    crl::multisense::DataSource src = stringToDataSource(dataSourceStr);

    bool status = cameraInterface->stopStreams(src);
    printf("Stopped stream %s status: %d\n", dataSourceStr.c_str(), status);
    this->modeChange = true;
}

void CRLPhysicalCamera::update(Base::Render render, crl::multisense::image::Header *pHeader) {


}

CRLBaseCamera::PointCloudData *CRLPhysicalCamera::getStream() {
    return meshData;
}

std::unordered_map<crl::multisense::DataSource, crl::multisense::image::Header> CRLPhysicalCamera::getImage() {
    return imagePointers;
}

CRLPhysicalCamera::~CRLPhysicalCamera() {

    if (meshData->vertices != nullptr)
        free(meshData->vertices);

}

CRLBaseCamera::CameraInfo CRLPhysicalCamera::getInfo() {
    return CRLBaseCamera::cameraInfo;
}

// Pick an image size
crl::multisense::image::Config CRLPhysicalCamera::getImageConfig() const {
    // Configure the sensor.
    crl::multisense::image::Config cfg;
    bool status = cameraInterface->getImageConfig(cfg);
    if (crl::multisense::Status_Ok != status) {
        printf("Failed to query image config: %d\n", status);
    }

    return cfg;
}

std::unordered_set<crl::multisense::DataSource> CRLPhysicalCamera::supportedSources() {
    // this method effectively restrics the supported sources for the classice libmultisense api
    std::unordered_set<crl::multisense::DataSource> ret;
    if (cameraInfo.supportedSources & crl::multisense::Source_Raw_Left) ret.insert(crl::multisense::Source_Raw_Left);
    if (cameraInfo.supportedSources & crl::multisense::Source_Raw_Right) ret.insert(crl::multisense::Source_Raw_Right);
    if (cameraInfo.supportedSources & crl::multisense::Source_Luma_Left) ret.insert(crl::multisense::Source_Luma_Left);
    if (cameraInfo.supportedSources & crl::multisense::Source_Luma_Right)
        ret.insert(crl::multisense::Source_Luma_Right);
    if (cameraInfo.supportedSources & crl::multisense::Source_Luma_Rectified_Left)
        ret.insert(crl::multisense::Source_Luma_Rectified_Left);
    if (cameraInfo.supportedSources & crl::multisense::Source_Luma_Rectified_Right)
        ret.insert(crl::multisense::Source_Luma_Rectified_Right);
    if (cameraInfo.supportedSources & crl::multisense::Source_Chroma_Aux)
        ret.insert(crl::multisense::Source_Chroma_Aux);
    if (cameraInfo.supportedSources & crl::multisense::Source_Chroma_Left)
        ret.insert(crl::multisense::Source_Chroma_Left);
    if (cameraInfo.supportedSources & crl::multisense::Source_Chroma_Right)
        ret.insert(crl::multisense::Source_Chroma_Right);
    if (cameraInfo.supportedSources & crl::multisense::Source_Disparity_Left)
        ret.insert(crl::multisense::Source_Disparity_Left);
    if (cameraInfo.supportedSources & crl::multisense::Source_Disparity_Right)
        ret.insert(crl::multisense::Source_Disparity_Right);
    if (cameraInfo.supportedSources & crl::multisense::Source_Disparity_Cost)
        ret.insert(crl::multisense::Source_Disparity_Cost);
    if (cameraInfo.supportedSources & crl::multisense::Source_Raw_Aux) ret.insert(crl::multisense::Source_Raw_Aux);
    if (cameraInfo.supportedSources & crl::multisense::Source_Luma_Aux) ret.insert(crl::multisense::Source_Luma_Aux);
    if (cameraInfo.supportedSources & crl::multisense::Source_Luma_Rectified_Aux)
        ret.insert(crl::multisense::Source_Luma_Rectified_Aux);
    if (cameraInfo.supportedSources & crl::multisense::Source_Chroma_Rectified_Aux)
        ret.insert(crl::multisense::Source_Chroma_Rectified_Aux);
    if (cameraInfo.supportedSources & crl::multisense::Source_Disparity_Aux)
        ret.insert(crl::multisense::Source_Disparity_Aux);
    return ret;
}

std::string CRLPhysicalCamera::dataSourceToString(crl::multisense::DataSource d) {
    switch (d) {
        case crl::multisense::Source_Raw_Left:
            return "Raw Left";
        case crl::multisense::Source_Raw_Right:
            return "Raw Right";
        case crl::multisense::Source_Luma_Left:
            return "Luma Left";
        case crl::multisense::Source_Luma_Right:
            return "Luma Right";
        case crl::multisense::Source_Luma_Rectified_Left:
            return "Luma Rectified Left";
        case crl::multisense::Source_Luma_Rectified_Right:
            return "Luma Rectified Right";
        case crl::multisense::Source_Chroma_Left:
            return "Color Left";
        case crl::multisense::Source_Chroma_Right:
            return "Source Color Right";
        case crl::multisense::Source_Disparity_Left:
            return "Disparity Left";
        case crl::multisense::Source_Disparity_Cost:
            return "Disparity Cost";
        case crl::multisense::Source_Jpeg_Left:
            return "Jpeg Left";
        case crl::multisense::Source_Rgb_Left:
            return "Source Rgb Left";
        case crl::multisense::Source_Lidar_Scan:
            return "Source Lidar Scan";
        case crl::multisense::Source_Raw_Aux:
            return "Raw Aux";
        case crl::multisense::Source_Luma_Aux:
            return "Luma Aux";
        case crl::multisense::Source_Luma_Rectified_Aux:
            return "Luma Rectified Aux";
        case crl::multisense::Source_Chroma_Aux:
            return "Color Aux";
        case crl::multisense::Source_Chroma_Rectified_Aux:
            return "Color Rectified Aux";
        case crl::multisense::Source_Disparity_Aux:
            return "Disparity Aux";
        default:
            return "Unknown";
    }
}

crl::multisense::DataSource CRLPhysicalCamera::stringToDataSource(const std::string &d) {
    if (d == "Raw Left") return crl::multisense::Source_Raw_Left;
    if (d == "Raw Right") return crl::multisense::Source_Raw_Right;
    if (d == "Luma Left") return crl::multisense::Source_Luma_Left;
    if (d == "Luma Right") return crl::multisense::Source_Luma_Right;
    if (d == "Luma Rectified Left") return crl::multisense::Source_Luma_Rectified_Left;
    if (d == "Luma Rectified Right") return crl::multisense::Source_Luma_Rectified_Right;
    if (d == "Color Left") return crl::multisense::Source_Chroma_Left;
    if (d == "Source Color Right") return crl::multisense::Source_Chroma_Right;
    if (d == "Disparity Left") return crl::multisense::Source_Disparity_Left;
    if (d == "Disparity Cost") return crl::multisense::Source_Disparity_Cost;
    if (d == "Jpeg Left") return crl::multisense::Source_Jpeg_Left;
    if (d == "Source Rgb Left") return crl::multisense::Source_Rgb_Left;
    if (d == "Source Lidar Scan") return crl::multisense::Source_Lidar_Scan;
    if (d == "Raw Aux") return crl::multisense::Source_Raw_Aux;
    if (d == "Luma Aux") return crl::multisense::Source_Luma_Aux;
    if (d == "Luma Rectified Aux") return crl::multisense::Source_Luma_Rectified_Aux;
    if (d == "Color Aux") return crl::multisense::Source_Chroma_Aux;
    if (d == "Color Rectified Aux") return crl::multisense::Source_Chroma_Rectified_Aux;
    if (d == "Disparity Aux") return crl::multisense::Source_Disparity_Aux;
    throw std::runtime_error(std::string{} + "Unknown Datasource: " + d);
}
