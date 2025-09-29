#include "main_menu.hpp"
#include "select_menu.hpp"

MainMenu::MainMenu()
{
    ReadConfigFile(&this->_fanCurveTable);

    // Initialize sensors
    InitializeSensors();

    // Initialize temperature and fan speed labels
    this->_socTempLabel = new tsl::elm::ListItem("SOC Temp: --C");
    this->_fanSpeedLabel = new tsl::elm::ListItem("Fan Speed: --%");

    this->_p0Label = new tsl::elm::ListItem("P0: " + std::to_string(this->_fanCurveTable->temperature_c) + "C | " + std::to_string((int)(this->_fanCurveTable->fanLevel_f * 100)) + "%");
    this->_p1Label = new tsl::elm::ListItem("P1: " + std::to_string((this->_fanCurveTable + 1)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 1)->fanLevel_f * 100)) + "%");
    this->_p2Label = new tsl::elm::ListItem("P2: " + std::to_string((this->_fanCurveTable + 2)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 2)->fanLevel_f * 100)) + "%");
    this->_p3Label = new tsl::elm::ListItem("P3: " + std::to_string((this->_fanCurveTable + 3)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 3)->fanLevel_f * 100)) + "%");
    this->_p4Label = new tsl::elm::ListItem("P4: " + std::to_string((this->_fanCurveTable + 4)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 4)->fanLevel_f * 100)) + "%");
    this->_p5Label = new tsl::elm::ListItem("P5: " + std::to_string((this->_fanCurveTable + 5)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 5)->fanLevel_f * 100)) + "%");
    this->_p6Label = new tsl::elm::ListItem("P6: " + std::to_string((this->_fanCurveTable + 6)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 6)->fanLevel_f * 100)) + "%");
    this->_p7Label = new tsl::elm::ListItem("P7: " + std::to_string((this->_fanCurveTable + 7)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 7)->fanLevel_f * 100)) + "%");
    this->_p8Label = new tsl::elm::ListItem("P8: " + std::to_string((this->_fanCurveTable + 8)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 8)->fanLevel_f * 100)) + "%");
    this->_p9Label = new tsl::elm::ListItem("P9: " + std::to_string((this->_fanCurveTable + 9)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 9)->fanLevel_f * 100)) + "%");

    if (IsRunning() != 0)
    {
        this->_enabledBtn = new tsl::elm::ToggleListItem("Enabled", true);
    }
    else
    {
        this->_enabledBtn = new tsl::elm::ToggleListItem("Enabled", false);
    }
}

MainMenu::~MainMenu()
{
    CloseSensors();
}

tsl::elm::Element* MainMenu::createUI()
{
    this->_tableIsChanged = false;

    auto frame = new tsl::elm::OverlayFrame("NX-FanControl", "v1.0.5+");

    auto list = new tsl::elm::List();

    this->_enabledBtn->setStateChangedListener([this](bool state)
    {
	    if (state)
        {
            CreateB2F();
            const NcmProgramLocation programLocation{
                .program_id = SysFanControlID,
                .storageID = NcmStorageId_None,
            };
            u64 pid = 0;
            pmshellLaunchProgram(0, &programLocation, &pid);
            return true;
		}
        else
        {
            RemoveB2F();
            pmshellTerminateProgram(SysFanControlID);
            return true;
        }
	    return false;
    });
    list->addItem(this->_enabledBtn);

    // Add real-time monitoring section
    list->addItem(new tsl::elm::CategoryHeader("Real-time Monitoring", true));
    list->addItem(this->_socTempLabel);
    list->addItem(this->_fanSpeedLabel);

    list->addItem(new tsl::elm::CategoryHeader("Fan Curve", true));
    this->_p0Label->setClickListener([this](uint64_t keys)
    {
	    if (keys & HidNpadButton_A) 
        {
			tsl::changeTo<SelectMenu>(0, this->_fanCurveTable, &this->_tableIsChanged);
			return true;
		}
		return false; 
    });
    list->addItem(this->_p0Label);

    this->_p1Label->setClickListener([this](uint64_t keys)
    {
	    if (keys & HidNpadButton_A) 
        {
			tsl::changeTo<SelectMenu>(1, this->_fanCurveTable, &this->_tableIsChanged);
			return true;
		}
		return false;
    });
    list->addItem(this->_p1Label);

    this->_p2Label->setClickListener([this](uint64_t keys)
    {
	    if (keys & HidNpadButton_A) 
        {
			tsl::changeTo<SelectMenu>(2, this->_fanCurveTable, &this->_tableIsChanged);
			return true;
		}
		return false; 
    });
    list->addItem(this->_p2Label);

    this->_p3Label->setClickListener([this](uint64_t keys)
    {
	    if (keys & HidNpadButton_A) 
        {
		    tsl::changeTo<SelectMenu>(3, this->_fanCurveTable, &this->_tableIsChanged);
			return true;
		}
		return false; 
    });
    list->addItem(this->_p3Label);

    this->_p4Label->setClickListener([this](uint64_t keys)
    {
	    if (keys & HidNpadButton_A) 
        {
				tsl::changeTo<SelectMenu>(4, this->_fanCurveTable, &this->_tableIsChanged);
				return true;
	    }
	    return false; 
    });
    list->addItem(this->_p4Label);
        this->_p5Label->setClickListener([this](uint64_t keys)
    {
        if (keys & HidNpadButton_A) 
        {
            tsl::changeTo<SelectMenu>(5, this->_fanCurveTable, &this->_tableIsChanged);
            return true;
        }
        return false;
    });
    list->addItem(this->_p5Label);

    this->_p6Label->setClickListener([this](uint64_t keys)
    {
        if (keys & HidNpadButton_A) 
        {
            tsl::changeTo<SelectMenu>(6, this->_fanCurveTable, &this->_tableIsChanged);
            return true;
        }
        return false;
    });
    list->addItem(this->_p6Label);

    this->_p7Label->setClickListener([this](uint64_t keys)
    {
        if (keys & HidNpadButton_A) 
        {
            tsl::changeTo<SelectMenu>(7, this->_fanCurveTable, &this->_tableIsChanged);
            return true;
        }
        return false;
    });
    list->addItem(this->_p7Label);

    this->_p8Label->setClickListener([this](uint64_t keys)
    {
        if (keys & HidNpadButton_A) 
        {
            tsl::changeTo<SelectMenu>(8, this->_fanCurveTable, &this->_tableIsChanged);
            return true;
        }
        return false;
    });
    list->addItem(this->_p8Label);

    this->_p9Label->setClickListener([this](uint64_t keys)
    {
        if (keys & HidNpadButton_A) 
        {
            tsl::changeTo<SelectMenu>(9, this->_fanCurveTable, &this->_tableIsChanged);
            return true;
        }
        return false;
    });
    list->addItem(this->_p9Label);

    frame->setContent(list);

    return frame;
}

void MainMenu::update()
{
    static u64 counter = 0;
    counter++;
    
    // Update readings every 6-12 frames (0.1-0.2s at 60 FPS)
    if (counter % 6 == 0) {
        // Get SOC temperature
        float socTemp = GetSOCTemperature();
        if (socTemp >= 0) {
            this->_socTempLabel->setText("SOC Temp: " + std::to_string((int)socTemp) + "C");
        } else {
            this->_socTempLabel->setText("SOC Temp: Error");
        }

        // Get fan speed
        float fanSpeed = GetFanSpeed();
        if (fanSpeed >= 0) {
            this->_fanSpeedLabel->setText("Fan Speed: " + std::to_string((int)fanSpeed) + "%");
        } else {
            this->_fanSpeedLabel->setText("Fan Speed: Error");
        }
    }

    if(this->_tableIsChanged)
    {
        this->_p0Label->setText("P0: " + std::to_string(this->_fanCurveTable->temperature_c) + "C | " + std::to_string((int)(this->_fanCurveTable->fanLevel_f * 100)) + "%");
        this->_p1Label->setText("P1: " + std::to_string((this->_fanCurveTable + 1)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 1)->fanLevel_f * 100)) + "%");
        this->_p2Label->setText("P2: " + std::to_string((this->_fanCurveTable + 2)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 2)->fanLevel_f * 100)) + "%");
        this->_p3Label->setText("P3: " + std::to_string((this->_fanCurveTable + 3)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 3)->fanLevel_f * 100)) + "%");
        this->_p4Label->setText("P4: " + std::to_string((this->_fanCurveTable + 4)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 4)->fanLevel_f * 100)) + "%");
        this->_p5Label->setText("P5: " + std::to_string((this->_fanCurveTable + 5)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 5)->fanLevel_f * 100)) + "%");
        this->_p6Label->setText("P6: " + std::to_string((this->_fanCurveTable + 6)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 6)->fanLevel_f * 100)) + "%");
        this->_p7Label->setText("P7: " + std::to_string((this->_fanCurveTable + 7)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 7)->fanLevel_f * 100)) + "%");
        this->_p8Label->setText("P8: " + std::to_string((this->_fanCurveTable + 8)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 8)->fanLevel_f * 100)) + "%");
        this->_p9Label->setText("P9: " + std::to_string((this->_fanCurveTable + 9)->temperature_c) + "C | " + std::to_string((int)((this->_fanCurveTable + 9)->fanLevel_f * 100)) + "%");

        this->_tableIsChanged = false;
    }
}