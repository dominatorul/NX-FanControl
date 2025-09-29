// main.cpp
// Define TESLA_INIT_IMPL before including tesla.hpp to get the implementations
#define TESLA_INIT_IMPL
#include <tesla.hpp>
#include "main_menu.hpp"

class NxFanControlOverlay : public tsl::Overlay {
public:
    virtual void initServices() override {
        // Tesla handles all the display/resolution setup automatically
        // You only initialize YOUR services here
        fsdevMountSdmc();
        pmshellInitialize();
    }
    
    virtual void exitServices() override {
        fsdevUnmountAll();
        pmshellExit();
    }

    virtual std::unique_ptr<tsl::Gui> loadInitialGui() override {
        return initially<MainMenu>();
    }
};

int main(int argc, char **argv) {
    // Tesla handles ALL the initialization, key combo detection, resolution, etc.
    // Your overlay just provides the GUI
    return tsl::loop<NxFanControlOverlay>(argc, argv);
}