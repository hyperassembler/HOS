#include <kahalu_utils.h>


void simulation_tb_wait(uint32_t cycles, const char * clock) {

    hw_sleep(cycles * 10); //Slowest clock for now.

}

uint32_t getRandInterval(uint32_t begin, uint32_t end) {
    uint32_t range = 1 + end - begin;
    uint32_t limit = RAND_MAX - (RAND_MAX % range); 

    uint32_t randVal;
    do {
        randVal = rand();
    } while (randVal >= limit);

    return (randVal % range) + begin;
}

void print_addr(volatile uint32_t* address){
    int print_val = 0;
    print_val = (intptr_t)address;
    hw_status("printval from function : %08x\n", print_val);
}

void set_reg(const int mc, uintptr_t addr, uint32_t field_mask, uint32_t data){
    
    uint32_t i, prog_val;
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);

    hw_status("Received addr = %08x, field_mask = %-8x, data = %08x\n", (addr + mc_base_addr), field_mask, data);
   
    if(field_mask != 0){
        //calculate how much we need to shift the data
        for(i=0; (((field_mask>>i)&1)==0); i++);
    }else{
        hw_status("Write mask is 0, returning without programming\n");
        return;
    }
  
    hw_status("Read Address = %08x\n", (addr + mc_base_addr));
    
    //get the data in the register    
    prog_val = hw_read32((uint32_t *)(addr + mc_base_addr));
    
    //clear the field we want to program
    prog_val = prog_val & (~field_mask);
    
    //put data in the field
    prog_val |= ((data<<i)&field_mask);
    
    hw_status("Writing Address = %08x With Value = %08x\n", (addr + mc_base_addr), prog_val);
    
    //program the value 
    hw_write32((uint32_t *)(addr + mc_base_addr), prog_val);
}

void set_reg_val(const int mc, uintptr_t addr, uint32_t field_mask, uint32_t data){
    
    uint32_t i, prog_val;
    uint32_t mc_base_addr = get_mcu_baseaddr(mc);
    
    hw_status("Set Reg Val Received addr = %08x, field_mask = %-8x, data = %08x\n", (addr + mc_base_addr), field_mask, data);
   
    if(field_mask != 0){
        //calculate how much we need to shift the data
        //for(i=0; (((field_mask>>i)&1)==0); i++);
    }else{
        hw_status("Write mask is 0, returning without programming\n");
        return;
    }
  
    hw_status("Read Address = %08x\n", (addr + mc_base_addr));
    
    //get the data in the register    
    prog_val = hw_read32((uint32_t *)(addr + mc_base_addr));
    hw_status("read back data = %08x",prog_val);
    //clear the field we want to program
    prog_val = prog_val & (~field_mask);
    hw_status("cleared read data = %08x",prog_val);
    
    //put data in the field
    prog_val |= (data & field_mask);
    
    hw_status("Writing Address = %08x With Value = %08x\n", (addr + mc_base_addr), prog_val);
    
    //program the value 
    hw_write32((uint32_t *)(addr + mc_base_addr), prog_val);
}

void do_yml_reg_writes(char *fname){
    uint32_t line_val, line_num, prog_addr, prog_data, prog_mask;
    char line[80];
    FILE *fr;            

    /*"rt" means open the file for reading text */
    /* open the file for reading */
    fr = fopen (fname, "rt");  
    
    if(fr==NULL){
        hw_status("Cound not find %s Skipping do_yml_reg_writes()\n", fname);
        return;
    }else{
        hw_status("Found %s, proceeding with do_yml_reg_writes()\n", fname);
    }
      
    line_num = 0;
    
    while(fgets(line, 80, fr) != NULL)
    {
        line_val =  (uint32_t)strtol(line, NULL, 2);
        hw_status ("Read line from file: %08x ", line_val);
                
        if(line_num == 0){
            //do nothing
            hw_status("num regs\n");
        }else if((line_num%3)==1){
            prog_addr = line_val;
            hw_status("addr\n");
        }else if((line_num%3)==2){
            prog_mask = line_val;
            hw_status("mask\n");
        }else if((line_num%3)==0){
            prog_data = line_val;
            hw_status("data\n");
            set_reg(0, prog_addr, prog_mask, prog_data);
        }
        line_num++;
    }
   
   fclose(fr);  /* close the file prior to exiting the routine */
}

#ifdef MCU_IP_VERIF
void get_current_dir(){
   char cwd[1024];
   if (getcwd(cwd, sizeof(cwd)) != NULL)
       fprintf(stdout, "Current working dir: %s\n", cwd);
   else
       perror("getcwd() error");
}
#endif //MCU_IP_VERIF


/*
UINT32 get_reg(volatile UINT32* addr, UINT32 field_mask){

    UINT32 ret_val = (hw_read32(addr) & field_mask);   
    
    if(field_mask == 0){
        return 0;
    } else{
        while((field_mask & 0x1)==0){
            field_mask >>= 1;
            ret_val >>= 1;
        }
    }
    return ret_val;
}
*/







