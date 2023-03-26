const FONTSET: [u8; 80] = 
[
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
];


pub struct Chocolate {
    memory: [u8; 0xFFF], // All Available Memory
    v: [u8; 16],  // Registers
    stack: [u16; 16], // Stack
    sp: u8, // Stack Pointer
    i: u16, // 16 bit register
    pc: u16, // Program counter
    delay: u8, // Delay Timer
    sound: u8, //Buzzer Timer
    buzzer: bool, //If buzzer is active
    draw: bool, //A boolean to re-draw the screen
    keymap: [u8; 16], // Stores what keys are being pressed
    graphics: [u8; 2048], //Screen Data
    pcDelta: i16 //Internal Program Counter increase delta
}


impl Chocolate {

    pub fn new() -> Chocolate {
        let mut chocolate = Chocolate{
            memory: [0; 0xFFF],
            v: [0; 16],
            stack: [0; 16],
            sp: 0,
            i: 0,
            pc: 0,
            delay: 0,
            sound: 0,
            buzzer: false,
            draw: false,
            keymap: [0; 16],
            graphics: [0; 2048],
            pcDelta: 1
        };
        for i in 0..80 {
            chocolate.memory[i] = FONTSET[i];
        }
        return chocolate;
    }

    /// Loads the ROM into memory
    fn load(&mut self, rom: &[i8]){
        for(i, value) in rom.iter().enumerate() {
            self.memory[0x200 + i] = *value as u8; 
        }
    }


    // Ticks the machine one clock cycle
    pub fn tick(&mut self, debug: bool) {
        //Get opcode

        let op = (self.memory[self.pc as usize] as u16) << 8 | self.memory[(self.pc +1) as usize] as u16;
        
        // Get operands
        let nnn = op & 0x0FFF;
        let nn = op & 0x00FF;
        let n = op & 0x000F;
        let x = (op & 0x0F00) >> 8;
        let y = (op & 0x00F0) >> 4;

        if debug {
            println!("{:#04X} : {:#04X}", self.pc, op);
            println!("NNN:{:#03X} NN:{:#02X} N:{:#X} X:{:#X} Y:{:#X}   I:{} ({:#03X})", nnn, nn, n, x, y, self.i, self.i);

        }

        //Reset the program counter delta
        let pc_delta = 1;
        match op & 0x00FF {
            0x00E0 => self.cls(),
            0x00EE => self.ret(),
            0x0000..=0x01FF => println!("SYS instruction ignored"),
            0x0200..=0x0FFF => println!("Unimplemented Opcode"),
            0x1000..=0x1FFF => println!("JP"),
            0x2000..=0x2FFF => println!("CALL"),
            0x3000..=0x3FFF => println!("SE"),
            0x4000..=0x4FFF => println!("SNE"),
            0x5000..=0x5FFF => println!("SE"),
            0x6000..=0x6FFF => println!("LD"),
            0x7000..=0x7FFF => println!("ADD"),
            0x8000..=0x8FFF => {
                match op & 0x000F {
                    0x0000 => println!("LD"),
                    0x0001 => println!("OR"),
                    0x0002 => println!("AND"),
                    0x0003 => println!("XOR"),
                    0x0004 => println!("ADD"),
                    0x0005 => println!("SUB"),
                    0x0006 => println!("SHR"),
                    0x0007 => println!("SUBN"),
                    0x000E => println!("SHL"),
                    _ => println!("Unknown Opcode"),
                }
            },
            0x9000..=0x9FFF => println!("SNE"),
            0xA000..=0xAFFF => println!("LD"),
            0xB000..=0xBFFF => println!("JP"),
            0xC000..=0xCFFF => println!("RND"),
            0xD000..=0xDFFF => println!("DRW"),
            0xE09E => println!("SKP"),
            0xE0A1 => println!("SKNP"),
            0xF007 => println!("LD"),
            0xF00A => println!("LD"),
            0xF015 => println!("LD"),
            0xF018 => println!("LD"),
            0xF01E => println!("ADD"),
            0xF029 => println!("LD"),
            0xF033 => println!("LD"),
            0xF055 => println!("LD"),
            0xF065 => println!("LD"),
            _ => println!("Unknown Opcode"),
        }

        if self.delay > 0 {
            self.delay -= 1;
        }

        if self.sound > 0 {
            self.sound -= 1; 
        }

    }

    /// 00E0 - CLS
    /// Clear the display. 
    fn cls(&mut self) {
        self.graphics.fill(0);
        self.draw = true; 
    }

    /// 00EE - RET
    /// Return from a subroutine.
    /// The interpreter sets the program counter to the address at the 
    /// top of the stack, then subtracts 1 from the stack pointer.
    fn ret(&mut self) {
        self.sp -= 1; 
        self.pc = self.stack[self.sp as usize];
    }

    /// 1nnn - JP addr
    /// Jump to location nnn.
    /// The interpreter sets the program counter to nnn.
    /// 
    fn jp(&mut self, nnn: u16){
        self.pc = nnn;
        self.pcDelta = 0; 
    }







}
