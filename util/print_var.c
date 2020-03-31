#define PRINT_NL serial->write('\n')
#define PRINT_TAB serial->write('\t')
#define PRINT_VAR(V) serial->print(V)
#define PRINT_CELL(V) PRINT_VAR(V); PRINT_TAB

void print_appSerial(AppSerial *item, HardwareSerial *serial){
	serial->print("app.serial.id: ");serial->print(item->id);serial->println(":");
	serial->print("\tkind: ");serial->print(serial_getAppKindStr(item->kind));serial->println(";");
}

void print_var(HardwareSerial *serial){
	extern App app;
	extern ChannelLList channels;
	extern AppSerial serials[];
	serial->print("app.id: ");serial->println(app.id);
	FOREACH_SERIAL(i)
		print_appSerial(&serials[i], serial);
	}
	
	serial->println("channel initial:");
	serial->println("id");
	FOREACH_CHANNEL(&channels)
		PRINT_CELL(channel->id);
		PRINT_NL;
	}
	
	serial->println("\nchannel runtime:");
	serial->println("id\tstate\terror\tsec_state");
	FOREACH_CHANNEL(&channels)
		PRINT_CELL(channel->id);
		PRINT_CELL(channel_getStateStr(channel));
		PRINT_CELL(channel_getErrorStr(channel));
		PRINT_NL;
	}
}

