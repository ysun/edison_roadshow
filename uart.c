#include "stdio.h"
//! [Interesting]
#include "mraa.h"

#include <unistd.h>
#include <fcntl.h>

#include <sys/select.h>

#define READ_TERMINAL 0
#define WRITE_TERMINAL 1
#define NUM_PRESS 5

mraa_uart_context uart;
mraa_aio_context adc_a0[NUM_PRESS];

struct ARG {
	int ano;
};

void mydelay(int n) {
    int z = 0;
    for(z = 0; z < n; z++);
}

int data_press[5] = {0};

char buffer[] = {'\xFF','\x55',
	'\x02','\x1','\x15',
	'\xEE'};

int do_main(int argc, char** argv);

static void sleep_ms(unsigned int secs) {
    struct timeval tval;
    tval.tv_sec=secs/1000;
    tval.tv_usec=(secs*1000)%1000000;
    select(0,NULL,NULL,NULL,&tval);
}

void thread_press(struct ARG *arg) {
    uint16_t adc_value = 0, max_value = 0, value_pre = 0,  value_cur = 0;
    char pre_high = 0;

//    int i = 0;
    while(1) {
	sleep_ms(100);
        adc_value = value_cur = mraa_aio_read(adc_a0[arg->ano]);

	if (arg->ano == 4) {
		value_cur = abs(adc_value - value_pre);

		if(value_cur > 50) {
			buffer[4] = value_cur;
			buffer[3] = arg->ano + 1;

			mraa_uart_write(uart, buffer, sizeof(buffer));
			printf("press: %d: %d\n", arg->ano, value_cur);
		}
		value_pre = adc_value;
	} else {
		if(adc_value > 50) {
			pre_high = 1;
			max_value = max_value > adc_value ? max_value: adc_value;
		} else {
			if(pre_high) {
				buffer[4] = max_value;
				buffer[3] = arg->ano + 1;
				mraa_uart_write(uart, buffer, sizeof(buffer));

				printf("press: %d: %d\n", arg->ano,  max_value);
			}
			max_value = adc_value;
			pre_high = 0;
		}
	}

//        adc_value_float = mraa_aio_read_float(adc_a0);

//	data_press[i%5] = adc_value;
//	i = (i + 1) %5;
    }
}

int main(int argc, char** argv)
{
    pthread_t pid_press[5];
    struct ARG arg_press[5];
    int ret = 0, i;


    //Init UART
    uart = mraa_uart_init(0);
    mraa_uart_set_baudrate(uart, 115200);
    if (uart == NULL) {
        fprintf(stderr, "UART failed to setup\n");
        return EXIT_FAILURE;
    }

    //test uart write!
    //mraa_uart_write(uart, buffer, sizeof(buffer));

    for(i = 0; i<NUM_PRESS; i++) {
	    //Init AIO
	    adc_a0[i] = mraa_aio_init(i);
	    if (adc_a0[i] == NULL) {
		    fprintf(stderr, "AIO0[%d] failed to setup\n", i);
	    }

	    //Create child thread!
            arg_press[i].ano = i;
	    ret = pthread_create(&pid_press[i], NULL, (void*)thread_press, (void*) &arg_press[i]);

	    if(ret != 0)
		    printf("Thread Create Error: %d\n", i);
    }

    pthread_join(pid_press[0], NULL);

//But, should not run here!
    pthread_join(pid_press[0], NULL);
    printf("Main Ends\n");
    mraa_uart_stop(uart);
    mraa_deinit();
//    mraa_aio_close(adc_a0);
    return EXIT_SUCCESS;
}

int do_main(int argc, char** argv) {
    return 0;
}

int aio() {
    return MRAA_SUCCESS;
}


/*
    while(1) {
         do_main(argc, argv);

        adc_value = mraa_aio_read(adc_a0);
        adc_value_float = mraa_aio_read_float(adc_a0);
        printf("%d - %f\n", adc_value, adc_value_float);
    }
*/

/*
    int pipe_press[2];
    pid_t    pid_press;
    char PipeBuf[64]={'a','0'};
    int read_ret=0;
    pipe(pipe_press);
    pid_press = fork();

    if (pid_press < 0) 
    {
        printf("fork error!\n");
        exit(1);
    }
    else if (pid_press == 0) 
    { //in child
        printf("Write in Pipe To FatherProcess!\n");
//        close(file_descriptors[READ_TERMINAL]);
        sleep(1);
        write(pipe_press[WRITE_TERMINAL],"Child Send",sizeof("Child Send"));
        //open(file_descriptors[READ_TERMINAL]);    
    }
    else 
    {
        printf("Read in Pipe From ChildProcess!\n");
        int flag = fcntl(pipe_press[READ_TERMINAL],F_GETFL,0);
        flag |= O_NONBLOCK;
        if(fcntl(pipe_press[READ_TERMINAL],F_SETFL,flag) < 0){
         perror("fcntl");
         exit(1);
        }
//        close(file_descriptors[WRITE_TERMINAL]);
        read_ret=read(pipe_press[READ_TERMINAL],PipeBuf,sizeof(PipeBuf));//没阻塞的读
        printf("Read Message are : %s\n",PipeBuf);
        
        read_ret=read(pipe_press[READ_TERMINAL],PipeBuf,sizeof(PipeBuf));
        printf("Read Message are : %s,ret=%d \n",PipeBuf,read_ret);    
        
    }
*/

