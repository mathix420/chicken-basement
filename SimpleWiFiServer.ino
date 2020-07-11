#include <ArduinoWebsockets.h>
#include <WiFi.h>

#define CLOSE_STATE		0
#define CLOSING_STATE	1
#define OPENNING_STATE	2
#define OPEN_STATE		3

#define BOTTOM_SWITCH	12
#define TOP_SWITCH		13
#define RELAY_1			2
#define RELAY_2			4

const char *ssid =		"Nordnet_GISSINGER";
const char *password =	"6R37UR3E";
volatile char DOOR_STATE = CLOSE_STATE;
volatile bool NOTIFIED = true;

using namespace websockets;
WebsocketsServer server;
WebsocketsClient client;

void	setup()
{
	Serial.begin(115200);
	pinMode(RELAY_1, OUTPUT);
	pinMode(RELAY_2, OUTPUT);
	pinMode(LED_BUILTIN, OUTPUT);
	attachInterrupt(TOP_SWITCH, openned_event, RISING);
	attachInterrupt(BOTTOM_SWITCH, closed_event, RISING);

	// Start LED for debugging
	digitalWrite(LED_BUILTIN, HIGH);

	// Connect to wifi
	WiFi.begin(ssid, password);

	// Wait some time to connect to wifi
	for (int i = 0; i < 15 && WiFi.status() != WL_CONNECTED; i++)
	{
		Serial.print(".");
		delay(1000);
	}

	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	server.listen(80);
	Serial.print("Is server live? ");
	Serial.println(server.available());

	// All done so LED is off now
	digitalWrite(LED_BUILTIN, LOW);
}

void	loop()
{
	if (!client.available())
		client = server.accept();
	else if (DOOR_STATE == OPEN_STATE || DOOR_STATE == CLOSE_STATE)
	{
		if (!NOTIFIED) notify_client();
		WebsocketsMessage msg = client.readBlocking();
		Serial.print("Got Message: ");
		Serial.println(msg.data());

		if (msg.data() == "OPEN_DOOR")
			client.send(open_door());
		else if (msg.data() == "CLOSE_DOOR")
			client.send(close_door());
		else if (msg.data())
			client.send("NOT_UNDERSTOOD");
	}
	delay(1000);
}

void	notify_client()
{
	if (DOOR_STATE == OPEN_STATE)
		client.send("DOOR_OPENED");
	else if (DOOR_STATE == CLOSE_STATE)
		client.send("DOOR_CLOSED");
}

char	*close_door()
{
	if (DOOR_STATE != OPEN_STATE)
		return "STATE_ERROR";
	digitalWrite(RELAY_1, HIGH);
	DOOR_STATE = CLOSING_STATE;
	return "CLOSING_DOOR";
}

char	*open_door()
{
	if (DOOR_STATE != CLOSE_STATE)
		return "STATE_ERROR";
	digitalWrite(RELAY_2, HIGH);
	DOOR_STATE = OPENNING_STATE;
	return "OPENNING_DOOR";
}

void	openned_event()
{
	if (DOOR_STATE != OPENNING_STATE)
		return
	digitalWrite(RELAY_2, LOW);
	digitalWrite(RELAY_1, LOW);
	DOOR_STATE = OPEN_STATE;
	NOTIFIED = false;
	Serial.println("DOOR_OPENNED");
}

void	closed_event()
{
	if (DOOR_STATE != CLOSING_STATE)
		return
	digitalWrite(RELAY_1, LOW);
	digitalWrite(RELAY_2, LOW);
	DOOR_STATE = CLOSE_STATE;
	NOTIFIED = false;
	Serial.println("DOOR_CLOSE");
}