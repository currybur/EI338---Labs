#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define PAGE_SIZE 256
#define PAGE_ENTRIES 256
#define FRAME_SIZE 256 
#define FRAME_ENTRIES 256
#define MEM_SIZE (FRAME_SIZE * FRAME_ENTRIES)
#define VIRTUAL_SIZE (PAGE_SIZE * PAGE_ENTRIES)
#define TLB_ENTRIES 16
#define OFFSET_BITS 8
#define OFFSET_MASK 255

int page_table[PAGE_ENTRIES];
int tlb[TLB_ENTRIES][2];
char memory[MEM_SIZE];
int mem_ptr = 0;
int faultNum = 0;
int tlbHit = 0;
int tlb_ptr = 0;

int get_page_number(int virtual) {
    return (virtual >> OFFSET_BITS) & (PAGE_ENTRIES-1);
}

int get_offset(int virtual) {
    return virtual & OFFSET_MASK;
}

int search_page_table(int page_number) {
    if (page_table[page_number] == -1) {
        faultNum++;
        return -1;
    }
    return page_table[page_number];
}

int search_tlb(int page_number) {
    for (int i = 0; i < TLB_ENTRIES; i++) {
        if (tlb[i][0] == page_number) {
            tlbHit++;
            return tlb[i][1];
        }
    }
    return -1;
}

void update_tlb(int page_number, int frame_number) {
    tlb[tlb_ptr][0] = page_number;
    tlb[tlb_ptr][1] = frame_number;
    tlb_ptr = (tlb_ptr + 1) % TLB_ENTRIES;
}

int main(int argc, const char *argv[])
{

    if (argc != 2){
        printf("<./file_exe_name> <input file>\n");
        exit(0);
    }

    for (int i=0; i<PAGE_ENTRIES; i++){
        page_table[i] = -1;
    }

    for (int i=0; i < TLB_ENTRIES; i++){
        tlb[i][0] = -1;
        tlb[i][1] = -1;
    }

    int addrNum = 0;
    int physical;
    int value;
    char buf[10];

    const char *store_file = "BACKING_STORE.bin";
    const char *input_file = argv[1];
    const char *out_file = "output.txt";

    int store_id = open(store_file, O_RDONLY);
    FILE *input_fp = fopen(input_file, "r");
    FILE *output_fp = fopen(out_file, "a");
    char *store_ptr = mmap(0, VIRTUAL_SIZE, PROT_READ, MAP_SHARED, store_id, 0);


    while(fgets(buf, 10, input_fp)!=NULL){
        addrNum++;
        int logical_addr = atoi(buf);
        int offset = get_offset(logical_addr);
        int page_number = get_page_number(logical_addr);

        int frame_number = search_tlb(page_number);
        if(frame_number!=-1){  // tlb hit
            physical = frame_number + offset;
            value = memory[physical];
        }
        else{
            frame_number = search_page_table(page_number);
            if(frame_number!=-1){
                physical = frame_number+offset;
                update_tlb(page_number, frame_number);
                value = memory[physical];
            }
            else{  // page fault
                int page_address = page_number * PAGE_SIZE;
                memcpy(memory + mem_ptr, store_ptr + page_address, PAGE_SIZE);
                frame_number = mem_ptr;
                for(int i=0;i<PAGE_ENTRIES;i++){
                    if(page_table[i]==frame_number){
                        page_table[i] = -1;
                    }
                }
                for (int i=0; i < TLB_ENTRIES; i++){
                    if(tlb[i][1] == frame_number){
                        tlb[i][0] = -1;
                        tlb[i][1] = -1;
                    }
                }
                physical = frame_number + offset;
                value = memory[physical];
                page_table[page_number] = frame_number;
                update_tlb(page_number, frame_number);
                mem_ptr = (mem_ptr + FRAME_SIZE) % MEM_SIZE;
            }
        }

        fprintf(output_fp, "Virtual address: %d ", logical_addr); 
        fprintf(output_fp, "Physical address: %d ", physical);
        fprintf(output_fp, "Value: %d\n", value);

    }

    float fault_rate = faultNum/(float)addrNum;
    float tlb_rate = tlbHit/(float) addrNum;

    fprintf(output_fp, "Number of Translated Addresses = %d\n", addrNum); 
    fprintf(output_fp, "Page Faults = %d\n", faultNum);
    fprintf(output_fp, "Page Fault Rate = %.3f\n", fault_rate);
    fprintf(output_fp, "TLB Hits = %d\n", tlbHit);
    fprintf(output_fp, "TLB Hit Rate = %.3f\n", tlb_rate);

    fclose(input_fp);
    fclose(output_fp);
    close(store_id);

    return 0;

}