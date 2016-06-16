#include <iostream>
//#include <string>
#include <typeinfo>

using namespace std;

typedef enum game_property
{
	PROPERTY_RUNNING,
	PROPERTY_USE_FPS_COUNTER,
	PROPERTY_ZOOMSCALE

} Game_Property;

typedef struct game_properties
{
	public:
		bool running;
		bool useFPSCounter;
		float zoomScale;

} Game_Properties;

static Game_Properties g_pGameProperties;

void game_throwPropertyError(string propertyName, string typeName);

template<typename T>
T game_getProperty(Game_Property property);
template<typename T>
T* game_getProperty(Game_Property property);

template<typename T>
void game_setProperty(Game_Property property, T object);
template<typename T>
void game_setProperty(Game_Property property, T* object);

template<typename T>
void game_setProperty(Game_Property property, T object)
{
	game_setProperty(property, &object);
}

template<typename T>
void game_setProperty(Game_Property property, T* object)
{
	switch (property)
	{
		case PROPERTY_RUNNING:
			if (typeid(T) != typeid(bool))
			{
				game_throwPropertyError("PROPERTY_RUNNING", "bool");
				break;
			}
			g_pGameProperties.running = (bool) *object;
			break;
		case PROPERTY_USE_FPS_COUNTER:
			if (typeid(T) != typeid(bool))
			{
				game_throwPropertyError("PROPERTY_USE_FPS_COUNTER", "bool");
				break;
			}
			g_pGameProperties.useFPSCounter = (bool) *object;
			break;
		case PROPERTY_ZOOMSCALE:
			if (typeid(T) != typeid(float))
			{
				game_throwPropertyError("PROPERTY_ZOOMSCALE", "float");
				break;
			}
			g_pGameProperties.zoomScale = (float) *object;
			break;
		default:
			cout << "[WARN] Unrecognized property!" << endl;
			break;
	}
}
