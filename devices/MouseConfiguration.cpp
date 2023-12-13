#include "MouseConfiguration.h"

MouseConfiguration::MouseConfiguration() : 
    CompositeDeviceConfiguration(MOUSE_REPORT_ID),
    _mouseButtonCount(5),
    _whichAxes{true, true, false, false, false, false, true, true}
{               
}

uint8_t MouseConfiguration::getDeviceReportSize()
{
    // TODO: Make number of mouse buttons dynamic
    uint8_t numOfMouseButtonBytes = this->getMouseButtonCount() / 8; // 5 hardcoded buttons for testing
    if (getMouseButtonPaddingBits() > 0)
    {
        numOfMouseButtonBytes++;
    }

    // TODO: Make number of mouse axis' dynamic? 
    uint8_t numOfMouseAxisBytes = this->getMouseAxisCount(); //X, Y, Wheel, Horiz wheel;

    // Mouse report size (bytes)
    mouseReportSize = numOfMouseButtonBytes + numOfMouseAxisBytes;
    return mouseReportSize;
}

void MouseConfiguration::makeDeviceReport(uint8* buffer, size_t size)
{
    uint8_t tempHidReportDescriptor[150];
    int hidReportDescriptorSize = 0;

    // Mouse setup
    tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE_PAGE(1);       
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01; //Generic Desktop

    tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1); 
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02; //Mouse

    tempHidReportDescriptor[hidReportDescriptorSize++] = COLLECTION(1);
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01; //Application

    tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1);
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01; //Pointer

    tempHidReportDescriptor[hidReportDescriptorSize++] = COLLECTION(1);
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x0; //Physical

    tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_ID(1);
    tempHidReportDescriptor[hidReportDescriptorSize++] = this->getMouseHidReportId(); //Mouse report ID
    
    // Buttons (Left, Right, Middle, Back, Forward)
    if (this->getMouseButtonCount() > 0)
    {
        tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE_PAGE(1);
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09; //USAGE_PAGE (Button)

        tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE_MINIMUM(1);
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01; //Button 1

        tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE_MAXIMUM(1);
        tempHidReportDescriptor[hidReportDescriptorSize++] = this->getMouseButtonCount();

        tempHidReportDescriptor[hidReportDescriptorSize++] = LOGICAL_MINIMUM(1);
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

        tempHidReportDescriptor[hidReportDescriptorSize++] = LOGICAL_MAXIMUM(1);
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

        tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_SIZE(1);
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

        tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_COUNT(1);
        tempHidReportDescriptor[hidReportDescriptorSize++] = this->getMouseButtonCount();

        tempHidReportDescriptor[hidReportDescriptorSize++] = HIDINPUT(1);
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02; //INPUT (Data, Variable, Absolute) ;5 button bits
        
        uint8_t mouseButtonPaddingBits = getMouseButtonPaddingBits();
        if (mouseButtonPaddingBits > 0)
        {
            // 5 buttons @ 1 bit each means we need 3 bits of padding to pad to a byte
            // The number of reports matches the number of bits needed to pad out to a byte
            tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_SIZE(1);
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

            tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_COUNT(1);
            tempHidReportDescriptor[hidReportDescriptorSize++] = mouseButtonPaddingBits;

            tempHidReportDescriptor[hidReportDescriptorSize++] = HIDINPUT(1);
            tempHidReportDescriptor[hidReportDescriptorSize++] = 0x03; //INPUT (Constant, Variable, Absolute)
        }
    }

    if (this->getMouseAxisCount() > 0)
    {
        // X/Y position, Wheel
        tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE_PAGE(1);
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01; //Generic Desktop

        tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1);
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x30; // X coordinate

        tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1);
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x031; // Y coordinate

        tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(1);
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x38; // Wheel

        tempHidReportDescriptor[hidReportDescriptorSize++] = LOGICAL_MINIMUM(1);
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81; // Logical Min (-127)

        tempHidReportDescriptor[hidReportDescriptorSize++] = LOGICAL_MAXIMUM(1);
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x7f; // Logical Max (127)

        tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_SIZE(1);
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x08; // Report Size (8). Whole byte, no padding needed

        tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_COUNT(1);
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x03; // Report Count (3). 3 bytes total

        tempHidReportDescriptor[hidReportDescriptorSize++] = HIDINPUT(1);
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x06; // Input (Data, Variable, Relative) ;3 bytes (X,Y,Wheel)
    
        // Horizontal wheel
        tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE_PAGE(1);
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x0c; //Consumer Devices

        tempHidReportDescriptor[hidReportDescriptorSize++] = USAGE(2);
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x38; //AC Pan
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;

        tempHidReportDescriptor[hidReportDescriptorSize++] = LOGICAL_MINIMUM(1);
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81; // Logical Min (-127)

        tempHidReportDescriptor[hidReportDescriptorSize++] = LOGICAL_MAXIMUM(1);
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x7f; // Logical Max (127)

        tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_SIZE(1);
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x08; // Report Size (8). Whole byte, no padding needed

        tempHidReportDescriptor[hidReportDescriptorSize++] = REPORT_COUNT(1);
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01; // Report Count (1). 1 byte total

        tempHidReportDescriptor[hidReportDescriptorSize++] = HIDINPUT(1);
        tempHidReportDescriptor[hidReportDescriptorSize++] = 0x06; // Input (Data, Variable, Relative) ;1 byte (Horizontal wheel)
    }

    // End Collection (Application - Physical)
    tempHidReportDescriptor[hidReportDescriptorSize++] = END_COLLECTION(0); //0xc0;

    // END_COLLECTION (Application)
    tempHidReportDescriptor[hidReportDescriptorSize++] = END_COLLECTION(0); //0xc0;


    if(hidReportDescriptorSize < size){
        memcpy(buffer, tempHidReportDescriptor, hidReportDescriptorSize);
    } else {
        ESP_LOGE("HID Report Descriptor size is larger than provided buffer size");
    }
}

uint16_t MouseConfiguration::getMouseButtonCount(){ return _mouseButtonCount; }
uint16_t MouseConfiguration::getMouseAxisCount(){ 
    int count = 0;
    for (int i = 0; i < POSSIBLEAXES; i++)
    {
        count += (int)_whichAxes[i];
    }

    return count;
}

void MouseConfiguration::setMouseButtonCount(uint16_t value) { _mouseButtonCount = value; }

MouseConfiguration::getMouseButtonPaddingBits()
{
    uint8_t mouseButtonPaddingBits = 8 - (this->getMouseButtonCount() % 8);
    if (mouseButtonPaddingBits == 8)
    {
        mouseButtonPaddingBits = 0;
    }

    return mouseButtonPaddingBits;
}