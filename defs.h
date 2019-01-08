#ifndef _defs
#define _defs


#define MIN_H 5												//mozne a predvolene rozmery sveta
#define MAX_H 60
#define DEF_H 25
#define MIN_W 5
#define MAX_W 130
#define DEF_W 50

#define HIST_SIZE 10										//velkost historie
#define GEN_BUFF_SIZE 5										//Max pocet generacii vypocitanych dopredu

#define PORT 12345											//port pouzity pri pripajani na server


enum {														//mozne stavy hry
	GAME_STATE_RUN, GAME_STATE_PAUSE, GAME_STATE_STEP, GAME_STATE_EDIT
};

enum {														//odpovede od servera
	SERVER_RESP_ERR, SERVER_RESP_OK, SERVER_RESP_NEW, SERVER_RESP_FILE_EXISTS, SERVER_RESP_FILE_NONEXISTS
};

enum {														//mozne poziadavky na server
	SERVER_CMD_SAVE, SERVER_CMD_LOAD, SERVER_CMD_USER, SERVER_CMD_FILENAME
};


typedef struct{												//na uchovavanie niekolko poslednych generacii
	char* cells[HIST_SIZE];
	unsigned short hist_pos;								//index posledneho platneho zaznamu v historii
	unsigned short hist_avail;								//pocet dostupnych zaznamov
} hist_t;

typedef struct{
	volatile unsigned width;
	volatile unsigned height;
	char *cells[GEN_BUFF_SIZE];
	volatile unsigned short gen_pos;
	volatile unsigned short read_pos;
	volatile char run_gen;
	pthread_t thread;
	pthread_mutex_t mutex;
	pthread_cond_t doRead;
	pthread_cond_t doGenerate;
} gen_struct_t;

typedef struct{												//vlastnosti a stav aktualneho sveta
	int w;													//rozmery sveta
	int h;
	char* cells;											//stav buniek
	unsigned generation;									//aktualna generacia
	char state;												//stav simulacie (pauza, beh, ...)
} world_t;

#endif
