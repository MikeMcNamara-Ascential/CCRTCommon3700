#include "DE10Nano.cpp"

int main(int argc, char *argv[]) 
{ 
    FILE *outFile = fopen("DE10NanoLogMain.txt", "w"); 
    fprintf(outFile, "DE10Nano::main(%d,%s)", argc, argv);
	
	DE10Nano *uut;
	uut = new DE10Nano();

    fprintf(outFile, "SetPortNum(25565)");
	uut->SetPortNum(25565);
    fprintf(outFile, "makeBatman()");
	uut->makeBatman();
	sleep(5);
    fprintf(outFile, "command"); 
	memcpy(uut->IPC.command,"conf msec 100",13);
	uut->IPC.response[0] = true;


    sleep(5);                                                                                                                                                                                        
    memcpy(uut->IPC.command,"send",4);                                                                                                                                                               
    uut->IPC.response[0] = true;                                                                                                                                                                     
                                                                                                                                                                                                     
    sleep(2);                                                                                                                                                                                        
    memcpy(uut->IPC.command,"stop",4);                                                                                                                                                               
    uut->IPC.response[0] = true;                                                                                                                                                                     
                                                                                                                                                                                                     
    sleep(2);                                                                                                                                                                                        
    for (int i = 0; i < uut->IPC.position[0];i++)                                                                                                                                                    
    {                                                                                                                                                                                                
        printf("LF: %u, RF: %u, LR: %u, RR: %u Index: %u\n\r",uut->sampler[i].channels[0],uut->sampler[i].channels[1],uut->sampler[i].channels[2],uut->sampler[i].channels[3],uut->sampler[i].index);
    }                                                                                                                                                                                                

	
	sleep(1);
	memcpy(uut->IPC.command,"gcon",4);
	uut->IPC.response[0] = true;

	sleep(1);
	memcpy(uut->IPC.command,"rset",4);
	uut->IPC.response[0] = true;

	sleep(1);
	memcpy(uut->IPC.command,"term",4);
	uut->IPC.response[0] = true;

    fclose(outFile); 
    fflush(outFile); 

	int i;
	sleep(1);
	exit(i);
}
