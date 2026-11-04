#include <stdint.h>
#include <arch/idt.h>
#include <arch/panic.h>

#define EXC_NOERR(fn, name) \
	static void __attribute__((interrupt, target("general-regs-only"))) fn(interrupt_frame_t* frame) { \
		panic_exception(name, frame, 0, false); \
	}

#define EXC_ERR(fn, name) \
	static void __attribute__((interrupt, target("general-regs-only"))) fn(interrupt_frame_t* frame, uint64_t error_code) { \
		panic_exception(name, frame, error_code, true); \
	}

EXC_NOERR(exc_de, "#DE: Divide Error")
EXC_NOERR(exc_db, "#DB: Debug Exception")
EXC_NOERR(exc_nmi, "NMI: Non-Maskable Interrupt")
EXC_NOERR(exc_bp, "#BP: Breakpoint")
EXC_NOERR(exc_of, "#OF: Overflow")
EXC_NOERR(exc_br, "#BR: Bound Range Exceeded")
EXC_NOERR(exc_ud, "#UD: Invalid Opcode")
EXC_NOERR(exc_nm, "#NM: Device Not Available")
EXC_ERR(exc_df, "#DF: Double Fault")
EXC_NOERR(exc_cso, "#CSO: Coprocessor Segment Overrun")
EXC_ERR(exc_ts, "#TS: Invalid TSS")
EXC_ERR(exc_np, "#NP: Segment Not Present")
EXC_ERR(exc_ss, "#SS: Stack-Segment Fault")
EXC_ERR(exc_gp, "#GP: General Protection Fault")
EXC_ERR(exc_pf, "#PF: Page Fault")
EXC_NOERR(exc_rsv, "#RSV: Reserved")
EXC_NOERR(exc_mf, "#MF: x87 Floating-Point Exception")
EXC_ERR(exc_ac, "#AC: Alignment Check")
EXC_NOERR(exc_mc, "#MC: Machine Check")
EXC_NOERR(exc_xm, "#XM: SIMD Floating-Point Exception")
EXC_NOERR(exc_ve, "#VE: Virtualization Exception")
EXC_ERR(exc_cp, "#CP: Control Protection Exception")
EXC_NOERR(exc_rsv22, "Reserved CPU exception")
EXC_NOERR(exc_rsv23, "Reserved CPU exception")
EXC_NOERR(exc_rsv24, "Reserved CPU exception")
EXC_NOERR(exc_rsv25, "Reserved CPU exception")
EXC_NOERR(exc_rsv26, "Reserved CPU exception")
EXC_NOERR(exc_rsv27, "Reserved CPU exception")
EXC_NOERR(exc_hv, "#HV: Hypervisor Injection Exception")
EXC_ERR(exc_vc, "#VC: VMM Communication Exception")
EXC_ERR(exc_sx, "#SX: Security Exception")
EXC_NOERR(exc_rsv31, "Reserved CPU exception")

static idt_entry_t idt[IDT_SIZE];
static idtr_t idt_pointer;

static void __attribute__((interrupt, target("general-regs-only"))) idt_default_handler(interrupt_frame_t* frame) {
	(void)frame;
}

static void idt_set_entry(uint8_t vector, void* handler) {
	uint64_t address = (uint64_t)handler;

	idt[vector].offset_0_15 = (uint16_t)(address & 0xFFFF);
	idt[vector].selector = KERNEL_CS;
	idt[vector].ist = 0;
	idt[vector].type_attr = INTERRUPT_GATE;
	idt[vector].offset_16_31 = (uint16_t)((address >> 16) & 0xFFFF);
	idt[vector].offset_32_63 = (uint32_t)(address >> 32);
	idt[vector].zero = 0;
}

void idt_init(void) {
	for (uint16_t i = 0; i < IDT_SIZE; i++) {
		idt_set_entry((uint8_t)i, (void*)idt_default_handler);
	}

	idt_set_entry(0, (void*)exc_de);
	idt_set_entry(1, (void*)exc_db);
	idt_set_entry(2, (void*)exc_nmi);
	idt_set_entry(3, (void*)exc_bp);
	idt_set_entry(4, (void*)exc_of);
	idt_set_entry(5, (void*)exc_br);
	idt_set_entry(6, (void*)exc_ud);
	idt_set_entry(7, (void*)exc_nm);
	idt_set_entry(8, (void*)exc_df);
	idt_set_entry(9, (void*)exc_cso);
	idt_set_entry(10, (void*)exc_ts);
	idt_set_entry(11, (void*)exc_np);
	idt_set_entry(12, (void*)exc_ss);
	idt_set_entry(13, (void*)exc_gp);
	idt_set_entry(14, (void*)exc_pf);
	idt_set_entry(15, (void*)exc_rsv);
	idt_set_entry(16, (void*)exc_mf);
	idt_set_entry(17, (void*)exc_ac);
	idt_set_entry(18, (void*)exc_mc);
	idt_set_entry(19, (void*)exc_xm);
	idt_set_entry(20, (void*)exc_ve);
	idt_set_entry(21, (void*)exc_cp);
	idt_set_entry(22, (void*)exc_rsv22);
	idt_set_entry(23, (void*)exc_rsv23);
	idt_set_entry(24, (void*)exc_rsv24);
	idt_set_entry(25, (void*)exc_rsv25);
	idt_set_entry(26, (void*)exc_rsv26);
	idt_set_entry(27, (void*)exc_rsv27);
	idt_set_entry(28, (void*)exc_hv);
	idt_set_entry(29, (void*)exc_vc);
	idt_set_entry(30, (void*)exc_sx);
	idt_set_entry(31, (void*)exc_rsv31);

	idt_pointer.limit = (uint16_t)(sizeof(idt) - 1);
	idt_pointer.base = (uint64_t)&idt;

	__asm__ volatile("lidt %0" : : "m"(idt_pointer));
}
