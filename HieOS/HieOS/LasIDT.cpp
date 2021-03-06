#include "LasIDT.h"
#include "memory.h"

static IDT_descripter	idt[256];
static IDTR idtr;

IDT_descripter * get_interrupt_descripter(uint16_t index) {
	return &idt[index];
}
static void IDTInstall() {
#ifdef _MSC_VER
	_asm lidt[_idtr]
#endif
}
__declspec(naked) void interrupt_default_handler() {
	_asm {
		PUSHAD
		PUSHFD
		CLI

		; EOI 신호를 보낸다
		; 여기서 시작
		PUSH EBP
		MOV EBP, ESP
		PUSH EAX

		MOV AL, 20H
		OUT DMA_PICU1, AL

		CMP BYTE PTR[EBP + 8], 7
		JBE END_OF_EOI
		OUT DMA_PICU2, AL

		END_OF_EOI :
		POP EAX
			POP EBP
			RET
		; 여기서 끝

		POPFD
		POPAD
		IRETD
	}
}

bool install_interrupt_handler(uint16_t index, uint16_t flag, uint16_t selecter, I86_IRQ_HANDLER handler)
{
	if (!handler)
		return false;

	uint64_t		uiBase = (uint64_t)&(*handler);

	if ((flag & 0x0500) == 0x0500) {
		idt[index].selector = selecter;
		idt[index].flag = flag;
	}
	else
	{
		idt[index].offset_low = uint16_t(uiBase & 0xffff);
		idt[index].offset_high = uint16_t((uiBase >> 16) & 0xffff);
		idt[index].reserved = 0;
		idt[index].flag = flag;
		idt[index].selector = selecter;
	}

	return	true;
}
bool IDT_init(uint16_t code_selector)
{

	idtr.base = (uint32_t)&idt[0];
	idtr.limit = sizeof(IDT_descripter) * 256 - 1;

	memset((void*)&idt[0], 0, sizeof(IDT_descripter) * 256 - 1);

	//디폴트 핸들러 등록
	for (int i = 0; i < 256; i++)
		install_interrupt_handler(i, 0x80 | 0x0e,
			code_selector, (I86_IRQ_HANDLER)interrupt_default_handler);

	//IDTR 레지스터를 셋업한다
	IDTInstall();

	return true;
}