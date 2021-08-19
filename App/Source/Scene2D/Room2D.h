struct Room2D
{
	enum DOOR_TYPE
	{
		DOOR_UP,
		DOOR_DOWN,
		DOOR_LEFT,
		DOOR_RIGHT,
		DOOR_TOTAL,				//must be last
	};

	DOOR_TYPE type;
	
	int prevRoom;
	int nextRoom;
	bool locked;

	Room2D(DOOR_TYPE = DOOR_UP);
	~Room2D();
};