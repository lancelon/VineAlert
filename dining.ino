#include "VineAlert.h"

using namespace RetailAlert;

SYSTEM_THREAD(ENABLED);

// PUB-SUB constants
constexpr char* DEVICE_NAME = "WOO_ALERT";
constexpr char* DOOR_MSSG = "DOOR";
constexpr char* DINING_MSSG = "DINING";

// constructor takes the time in milliseconds, callback function and true if you want it to repeat.
MillisTimer relaytimer(1000, relayCallback, false);
MillisTimer doorbelltimer(60000, doorbellcallback, false);


/*
create IndicatorLED objects
constructor expects the LED pin */
IndicatorLED doorbellLED(D7);
IndicatorLED relayPin(D1);

const int DoorReed = D5; 

int DoorBellPushed = 0;
int reedswitch = 0;
int DoorAnswered = 0;

/*
Create ButtonPressAction objects
Constructor expects a pointer to function that executes on button press for example:
ButtonPressAction someButtonName(someFunction);
here we are just using lambdas */
ButtonPressAction doorButton([](){
    if (DoorBellPushed == 0)
	{
	  //Particle.publish(DEVICE_NAME, DOOR_MSSG, 60, PRIVATE);
      Serial << "Doorbell rung\n";
      doorbellLED.on();
	  doorbelltimer.start();
	  DoorAnswered = 0;
	  DoorBellPushed = 1;
	  relayPin.on();
	  relaytimer.start();
	}
});



ButtonPressAction DININGButton([](){
    Particle.publish(DEVICE_NAME, DINING_MSSG, 60, PRIVATE);
    Serial << "DINING call button rung\n";
});

// function declarations
void eventHandler(const char* event, const char* data);


void setup()
{
  Particle.subscribe(DEVICE_NAME, eventHandler, MY_DEVICES);
  Serial.begin(9600);

  uint32_t startMillis = millis();
  while (millis() - startMillis < 5000){
  }

  doorButton.bindToPin(D0);
  DININGButton.bindToPin(D2);
  pinMode(DoorReed, INPUT);
  digitalWrite(DoorReed, HIGH);           // turn on reed input pin's pull-up resistor
  doorbellLED.begin();
  relayPin.begin();
  doorbelltimer.begin();
  doorbelltimer.start(false);
  relaytimer.begin();
  relaytimer.start(false);
}

void loop()
{
  reedswitch = digitalRead(DoorReed);
  MillisTimer::processTimers();
  ButtonPressAction::update();
  ToggleSwitchAction::update();
  if (reedswitch == HIGH && DoorBellPushed == 1)
    {
	  DoorAnswered = 1;
      doorbellcallback();
	  DoorBellPushed = 0;
      Serial << "Doorbell answered\n";
	}
   if (reedswitch == HIGH && DoorBellPushed == 0)
   {
	   //Serial << "Door opened with key";
   }
      if (reedswitch == LOW && DoorBellPushed == 1)
   {
	   //Serial << "Waiting for an answer";
   }
   if (reedswitch == LOW && DoorBellPushed == 0)
   {
	   //Serial << "Nothin' doing";
   }
}

void eventHandler(const char* event, const char* data)
{
  if(strcmp(data, DOOR_MSSG) == 0)
  {
    Serial << "Front Door Button\n";
    doorbellLED.on();
  }
  else if (strcmp(data, DINING_MSSG) == 0)
  {
    Serial << "DINING Button\n";
  }
}


void relayCallback (void){
	relaytimer.stop();
	relayPin.off();
	Serial << "Relay Off!!\n";
}

void doorbellcallback (void){
    doorbellLED.off();
	DoorBellPushed = 0;
	Serial << "Doorbell off!!\n";
    doorbelltimer.stop();
	if (DoorAnswered == 0)
	{Serial << "Door went unanswered\n";
	}
}


