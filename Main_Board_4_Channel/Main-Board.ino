/* Includes ------------------------------------------------------------------*/
#include "NCD4Relay/NCD4Relay.h"

NCD4Relay relayController;

SYSTEM_MODE(AUTOMATIC);

int triggerRelay(String command);

bool tripped[4];

int debugTrips[4];

int minTrips = 5;

void board1Handler(const char *event, const char *data);

void board2Handler(const char *event, const char *data);

void board3Handler(const char *event, const char *data);

/* This function is called once at start up ----------------------------------*/
void setup()
{
    Particle.function("controlRelay", triggerRelay);
    Serial.begin(115200);
    relayController.setAddress(0,0,0);
    Particle.subscribe("Board1_1", board1Handler, "2c0043000647343339373536");
    Particle.subscribe("Board2_1", board2Handler, "450031001847343337363432");
    Particle.subscribe("Board3_1", board3Handler, "250046001547343339383037");
}

/* This function loops forever --------------------------------------------*/
void loop()
{
    int status = relayController.readAllInputs();
    int a = 0;
    for(int i = 1; i < 9; i*=2){
        if(status & i){
            debugTrips[a]++;
            if(debugTrips[a] >= minTrips){
                if(!tripped[a]){
                    tripped[a] = true;
                    //set input trip event to true
                    String eventName = "Main_";
                    eventName+=(a+1);
                    Particle.publish(eventName, "ON");
                }
            }
        }else{
            debugTrips[a] = 0;
            if(tripped[a]){
                tripped[a] = false;
                //set input trip event to false
                String eventName = "Main_";
                eventName+=(a+1);
                Particle.publish(eventName, "OFF");
            }
        }
        a++;
    }
}

int triggerRelay(String command){
    if(command.equalsIgnoreCase("turnonallrelays")){
        relayController.turnOnAllRelays();
        return 1;
    }
    if(command.equalsIgnoreCase("turnoffallrelays")){
        relayController.turnOffAllRelays();
        return 1;
    }
    if(command.startsWith("setBankStatus:")){
        int status = command.substring(14).toInt();
        if(status < 0 || status > 255){
            return 0;
        }
        Serial.print("Setting bank status to: ");
        Serial.println(status);
        relayController.setBankStatus(status);
        Serial.println("done");
        return 1;
    }
    //Relay Specific Command
    int relayNumber = command.substring(0,1).toInt();
    Serial.print("relayNumber: ");
    Serial.println(relayNumber);
    String relayCommand = command.substring(1);
    Serial.print("relayCommand:");
    Serial.print(relayCommand);
    Serial.println(".");
    if(relayCommand.equalsIgnoreCase("on")){
        Serial.println("Turning on relay");
        relayController.turnOnRelay(relayNumber);
        Serial.println("returning");
        return 1;
    }
    if(relayCommand.equalsIgnoreCase("off")){
        relayController.turnOffRelay(relayNumber);
        return 1;
    }
    if(relayCommand.equalsIgnoreCase("toggle")){
        relayController.toggleRelay(relayNumber);
        return 1;
    }
    if(relayCommand.equalsIgnoreCase("momentary")){
        relayController.turnOnRelay(relayNumber);
        delay(300);
        relayController.turnOffRelay(relayNumber);
        return 1;
    }
    return 0;
}

void board1Handler(const char *event, const char *data)
{
    String command = String(data);
    if(command.equalsIgnoreCase("ON")){
        relayController.turnOnRelay(1);
    }
    
    if(command.equalsIgnoreCase("OFF")){
        relayController.turnOffRelay(1);
    }
}

void board2Handler(const char *event, const char *data)
{
    String command = String(data);
    if(command.equalsIgnoreCase("ON")){
        relayController.turnOnRelay(2);
    }
    
    if(command.equalsIgnoreCase("OFF")){
        relayController.turnOffRelay(2);
    }
}

void board3Handler(const char *event, const char *data)
{
    String command = String(data);
    if(command.equalsIgnoreCase("ON")){
        relayController.turnOnRelay(3);
    }
    
    if(command.equalsIgnoreCase("OFF")){
        relayController.turnOffRelay(3);
    }
}
