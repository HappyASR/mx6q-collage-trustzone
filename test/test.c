typedef unsigned long  u32;
typedef unsigned short u16;
typedef unsigned char  u8;
#define __REG(x)        (*((volatile u32 *)(x)))
#define __REG16(x)      (*((volatile u16 *)(x)))
#define __REG8(x)       (*((volatile u8 *)(x)))

#define LED_ON	1
#define LED_OFF	0

#define Asm __asm__ volatile
#define CP15_GET_SCR(x) 	Asm("mrc p15, 0, %0, c1, c1, 0":"=r"(x))
#define CP15_GET_NSACR(x) 	Asm("mrc p15, 0, %0, c1, c1, 2":"=r"(x))
#define CP15_GET_CPACR(x) 	Asm("mrc p15, 0, %0, c1, c0, 2":"=r"(x))
#define CP15_GET_MIDR(x) 	Asm("mrc p15, 0, %0, c0, c0, 0":"=r"(x))

#define CP15_SET_SCR(x)		Asm("mcr p15, 0, %0, c1,  c1, 0"::"r"(x))
#define CP15_SET_VBAR(x) 	Asm("mcr p15, 0, %0, c12, c0, 0"::"r"(x))
#define ISB() Asm("isb")

#define ID_RUN_UBOOT		(0xF0)
#define ID_SWITCH_TO_NS		(0xF4)
#define ID_SWITCH_TO_S		(0xF8)
#define ID_EXE_CMD			(0xFC)
#define ID_RUN_PRO			(0xE0)

extern void _smc_invoke();

void led_ctrl(u32 led_flag)
{
    u32 led_reg, i;
    for( i=0; i<0x100000; i++ );

    led_reg = 0x020b4004;
    if( LED_ON == led_flag ){
        __REG(led_reg) = 0x0;
    }
    else {
        __REG(led_reg) = 0x3;
    }
}

void (*printf)(char *,...) = 0x27812cd8;

int main(void)
{
	unsigned int reg;
	
	register unsigned int arg_r0 asm("r0") = ID_EXE_CMD;
	register unsigned int arg_r1 asm("r1") = 2;
	register unsigned int arg_r2 asm("r2") = 3;
	register unsigned int arg_r3 asm("r3") = 4;
	
	
	CP15_GET_NSACR(reg);
	printf("NSACR = %08x\n", reg);
	
	CP15_GET_CPACR(reg);
	printf("CPACR = %08x\n", reg);
	
		
	//CP15_GET_SCR(reg);
	//printf("SCR = %08x\n", reg);
	
	//ISB();
	while(1){
		led_ctrl(LED_OFF);
		//_smc_invoke();
		printf("invoke smc\n");
		//asm volatile ("smc #0\n\t");
#if 0		
		arg_r0 = ID_EXE_CMD;
		__asm__ volatile (".arch_extension sec\n\t"                               
						"dsb\n\t"                                               
						"smc #0\n\t"                                            
						: "=r" (arg_r0)                                         
						: "r" (arg_r0), "r" (arg_r1), "r" (arg_r2), "r" (arg_r3)
						: "r4", "r5", "r6", "r7", "r8", "r9", "r10", "ip", "lr", \
						"memory", "cc");
#endif						
				
	}
	
	return 0;
}

#define ICDDCR		0x00a01000
#define ICDISR0		0x00a01080
#define ICDISRnS	0x00a01084
#define ICDISRnE	0x00a01094
#define ICDSGIR		0x00a01f00
#define ICCICR		0x00a00100
#define ICCBPR		0x00a00104


void trustzone_setting(void)
{
	u32 reg;
	
	// CSU
	for(reg = 0x021C0000; reg < 0x021C00A0; reg = reg + 4)
		__REG(reg) = 0x00ff00ff; 

	// SCU 	
	reg = 0x00a00050;
	__REG(reg) = 0xf;  
	
	reg = 0x00a00054;
	__REG(reg) = 0xfff;   
	
	// GIC	
	// ICDISR Secure Interrupt Security Registers
	reg = ICDISR0;
	__REG(reg) = 0xf800ffff;
	
	for(reg = ICDISRnS; reg < ICDISRnE; reg = reg + 4)
		__REG(reg) = 0xffffffff; 

#if 1		
	// ICDDCR Banked Distributor Control Register
	reg = ICDDCR;
	__REG(reg) = 0x3;
	
	reg = ICDSGIR;
	__REG(reg) = 1<<15;
#endif	
	// ICCBPR Banked Binary Point Register
	reg = ICCBPR;
	__REG(reg) = 0xf8;
#if 1	
	// ICCICR Banked CPU Interface Control Register
	reg = ICCICR;
	__REG(reg) = 0xf;
#endif	

	// L2_Cache setting
	reg = 0x00a02000 + 0x108;
	__REG(reg) = 0x132;
	
	reg = 0x00a02000 + 0x10c;
	__REG(reg) = 0x132;
	
	reg = 0x00a02000 + 0xf60;
	__REG(reg) = 0x40800000;
	
	reg = 0x00a02000 + 0xf80;
	__REG(reg) = 0x3;
}