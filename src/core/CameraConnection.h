//
// Created by magnus on 3/21/22.
//

#ifndef MULTISENSE_CAMERACONNECTION_H
#define MULTISENSE_CAMERACONNECTION_H

#include <MultiSense/src/imgui/Layer.h>
#include <MultiSense/src/crl_camera/CRLBaseInterface.h>

/**
 * Class handles the bridge between the GUI interaction and actual communication to camera
 * Also handles all configuration with local network adapter
 */
class CameraConnection
{
public:
    CameraConnection();
    ~CameraConnection();

    struct CamPreviewBar {
        bool active = false;
        uint32_t numQuads = 3;

    }camPreviewBar;

    /** @brief Handle to the current camera object */
    CRLBaseInterface *camPtr = nullptr;
    bool preview = false;
    std::string lastActiveDevice{};

    void onUIUpdate(std::vector<Element> *pVector);

private:
    int sd = 0;

    Element* currentActiveDevice = nullptr; // handle to current device

    void updateActiveDevice(Element element);

    void connectCrlCamera(Element &element);

    void updateDeviceState(Element *element);

    void disableCrlCamera(Element &element);

    void setNetworkAdapterParameters(Element &dev);

    void setStreamingModes(Element &dev);
};


#endif //MULTISENSE_CAMERACONNECTION_H
