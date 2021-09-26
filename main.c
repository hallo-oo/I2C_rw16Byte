#include <stdio.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#include "typedef.h"


#define I2C_FILE_NAME "/dev/i2c-0"
#define USAGE_MESSAGE \
    "Usage:\n" \
    "  %s i2c_ch r [addr] [register]   " \
        "to read value from [register]\n" \
    "  %s i2c_ch w [addr] [register] [value]   " \
        "to write a value [value] to register [register]\n" \



static int set_i2c_register(int file,
                            unsigned char addr,
                            unsigned char reg,
                            unsigned char *value) {
 
    unsigned char outbuf[3];
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[1];
 
    messages[0].addr  = addr;
    messages[0].flags = 0;//write
    messages[0].len   = sizeof(outbuf);
    messages[0].buf   = outbuf;
 
    /* The first byte indicates which register we'll write */
    outbuf[0] = reg;
 
    /* 
     * The second byte indicates the value to write.  Note that for many
     * devices, we can write multiple, sequential registers at once by
     * simply making outbuf bigger.
     */
    outbuf[1] = value[0];
	outbuf[2] = value[1];
 
    /* Transfer the i2c packets to the kernel and verify it worked */
    packets.msgs  = messages;
    packets.nmsgs = 1;
    if(ioctl(file, I2C_RDWR, &packets) < 0) 
	{
        perror("Unable to send data");
        return 1;
    }
 
    return 0;
}
 
static int get_i2c_register(int file,
                            unsigned char addr,
                            unsigned char reg,
                            unsigned char *val) {
    unsigned char inbuf, outbuf;
    struct i2c_rdwr_ioctl_data packets;
    struct i2c_msg messages[2];
 	unsigned char buffer[2] = {0};
    /*
     * In order to read a register, we first do a "dummy write" by writing
     * 0 bytes to the register we want to read from.  This is similar to
     * the packet in set_i2c_register, except it's 1 byte rather than 2.
     */
    outbuf = reg;
    messages[0].addr  = addr;
    messages[0].flags = 0;	//write
    messages[0].len   = 2;
    messages[0].buf   = &outbuf;
 
    /* The data will get returned in this structure */
    messages[1].addr  = addr;
    messages[1].flags = I2C_M_RD/* | I2C_M_NOSTART*/;
    messages[1].len   = 2;
    messages[1].buf   = val;
 
    /* Send the request to the kernel and get the result back */
    packets.msgs      = messages;
    packets.nmsgs     = 2;
    if(ioctl(file, I2C_RDWR, &packets) < 0) {
        perror("Unable to send data");
        return 1;
    }
    //*val = buffer;
 
    return 0;
}
 
int main(int argc, char **argv) 
{
    int i2c_file;
	char i2cDevName[15] = {0};
    
    if(argc > 4 && !strcmp(argv[2], "r")) {
		
		int i2c_ch = strtol(argv[1], NULL, 0);
		if( i2c_ch > 10 || i2c_ch < 0 ){
			perror("input argv[1] i2c ch error\n");
			exit(1);
		}
		sprintf(i2cDevName,"/dev/i2c-%d",i2c_ch);
 
		// Open a connection to the I2C userspace control file.
		if ((i2c_file = open(i2cDevName, O_RDWR)) < 0) {
			perror("Unable to open i2c control file");
			exit(1);
		}
		
        int addr = strtol(argv[3], NULL, 0);
        int reg = strtol(argv[4], NULL, 0);
        unsigned char value[2];
		U16 dac_val;
        if(get_i2c_register(i2c_file, addr, reg, value)) {
            printf("Unable to get register!\n");
        }
        else 
		{	
			dac_val=ARR_TO_U16_BE(value);
            printf("Register = 0x%x, dac_val=0x%04x \n", reg, dac_val);
        }
    }
    else if(argc > 5 && !strcmp(argv[2], "w")) {
		int i2c_ch = strtol(argv[1], NULL, 0);
		if( i2c_ch > 10 || i2c_ch < 0 ){
			perror("input argv[1] i2c ch error\n");
			exit(1);
		}
		sprintf(i2cDevName,"/dev/i2c-%d",i2c_ch);
 
		// Open a connection to the I2C userspace control file.
		if ((i2c_file = open(i2cDevName, O_RDWR)) < 0) {
			perror("Unable to open i2c control file");
			exit(1);
		}
		
        int addr = strtol(argv[3], NULL, 0);
        int reg = strtol(argv[4], NULL, 0);
        int value = strtol(argv[5], NULL, 0);
		unsigned char dac_val[2];
		U16_TO_ARR_BE(value,dac_val);
		//printf("dac_val[0]=0x%x dac_val[1]=0x%x",dac_val[0],dac_val[1]);
        if(set_i2c_register(i2c_file, addr, reg, dac_val)) {
            printf("Unable to get register!\n");
        }
        else 
		{
            printf("Set register 0x%x: value=0x%x\n", reg, value);
        }
    }
    else {
        fprintf(stderr, USAGE_MESSAGE, argv[0], argv[0]);
    }
 
    close(i2c_file);
 
    return 0;
}



