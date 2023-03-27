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
    pc_delta: i16 //Internal Program Counter increase delta
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
            pc_delta: 1
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
        let kk = (op & 0x00FF) as u8;
        let n = (op & 0x000F) as u8;
        let x = ((op & 0x0F00) >> 8) as u8;
        let y = ((op & 0x00F0) >> 4) as u8;

        if debug {
            println!("{:#04X} : {:#04X}", self.pc, op);
            println!("NNN:{:#03X} KK:{:#02X} N:{:#X} X:{:#X} Y:{:#X}   I:{} ({:#03X})", nnn, kk, n, x, y, self.i, self.i);

        }

        //Reset the program counter delta
        self.pc_delta = 1;
        match op & 0x00FF {
            0x00E0 => self.cls(),
            0x00EE => self.ret(),
            0x0000..=0x01FF => println!("SYS instruction ignored"),
            0x0200..=0x0FFF => println!("Unimplemented Opcode"),
            0x1000..=0x1FFF => self.jp(nnn),
            0x2000..=0x2FFF => self.call(nnn),
            0x3000..=0x3FFF => self.se(x, kk),
            0x4000..=0x4FFF => self.sne(x, kk),
            0x5000..=0x5FFF => self.se_reg(x, y),
            0x6000..=0x6FFF => self.ld(x, kk),
            0x7000..=0x7FFF => self.add(x, kk),
            0x8000..=0x8FFF => {
                match op & 0x000F {
                    0x0000 => self.ld_reg_reg(x, y),
                    0x0001 => self.or(x, y),
                    0x0002 => self.and(x, y),
                    0x0003 => self.xor(x, y),
                    0x0004 => self.add_carry(x, y),
                    0x0005 => self.sub(x, y),
                    0x0006 => self.shr(x),
                    0x0007 => self.subn(x, y),
                    0x000E => self.shl(x),
                    _ => println!("Unknown Opcode"),
                }
            },
            0x9000..=0x9FFF => self.sne_register(x, y),
            0xA000..=0xAFFF => self.ld_i(nnn),
            0xB000..=0xBFFF => self.jmp_memory(nnn),
            0xC000..=0xCFFF => self.rnd(x, kk),
            0xD000..=0xDFFF => self.drw(x,y,n),
            0xE09E => self.skp(x),
            0xE0A1 => self.sknp(x),
            0xF007 => self.ld_reg_delay(x),
            0xF00A => self.ld_key(x),
            0xF015 => self.ld_delay_reg(x),
            0xF018 => self.ld_sound(x),
            0xF01E => self.add_i(x),
            0xF029 => self.ld_font(x),
            0xF033 => self.ld_bcd(x),
            0xF055 => self.ld_mem(x),
            0xF065 => self.ld_reg_mem(x),
            _ => println!("Unknown Opcode"),
        }

        if self.delay > 0 {
            self.delay -= 1;
        }

        if self.sound > 0 {
            self.sound -= 1; 
        }

    }

    /// # ``00E0`` - CLS
    /// Clear the display. 
    fn cls(&mut self) {
        self.graphics.fill(0);
        self.draw = true; 
    }

    /// # ``00EE`` - RET
    /// Return from a subroutine.
    /// The interpreter sets the program counter to the address at the top of the stack, then subtracts 1 from the stack pointer.
    fn ret(&mut self) {
        self.sp -= 1; 
        self.pc = self.stack[self.sp as usize];
    }

    /// # ``1nnn`` - JP addr
    /// The interpreter sets the program counter to nnn.
    /// ## Arguments
    /// - ``nnn`` Jump to location nnn.
    fn jp(&mut self, nnn: u16){
        self.pc = nnn;
        self.pc_delta = 0; 
    }

    /// # ``2nnn`` - CALL addr
    /// The interpreter increments the stack pointer, then puts the current PC on the top of the stack. The PC is then set to nnn.
    /// ## Arguments
    /// - ``nnn`` Call subroutine at nnn
    fn call(&mut self, nnn: u16){
        self.stack[self.sp as usize] = self.pc;
        self.sp += 1;
        self.pc = nnn;
        self.pc_delta = 0; 
    }

    /// # ``3xkk`` - SE Vx, byte
    /// The interpreter compares register Vx to nn, and if they are equal, increments the program counter by 2.
    /// ## Arguments
    /// - ``x`` Register to compare
    /// - ``nn`` Value to compare
    fn se(&mut self, x: u8, kk: u8){
       if self.v[x as usize] == kk {self.pc_delta = 2}; 
    }

    /// # ``4xkk`` - SNE Vx, byte
    /// The interpreter compares register Vx to kk, and if they are not equal, increments the program counter by 2.
    /// ## Arguments
    /// - ``x`` Register to compare
    /// - ``nn`` Value to compare
    fn sne(&mut self, x: u8, kk:u8){
        if self.v[x as usize] != kk {self.pc_delta = 2}; 
    }

    /// # ``5xy0`` - SE Vx, Vy
    /// The interpreter compares register Vx to register Vy, and if they are equal, increments the program counter by 2.
    /// ## Arguments
    /// - ``x`` Register to compare
    /// - ``y`` Register to compare
    fn se_reg(&mut self, x: u8, y: u8){
        if self.v[x as usize] == self.v[y as usize] {self.pc_delta = 2}
    }

    /// # ``6xkk`` - LD Vx, byte
    /// The interpreter puts the value kk into register Vx
    /// ## Arguments
    /// - ``x`` Register to fill
    /// - ``kk`` Value
    fn ld(&mut self, x: u8, kk: u8){
        self.v[x as usize] = kk;     
    }

    /// # ``7xkk`` - ADD Vx, byte
    /// Set Vx = Vx + kk.
    /// Adds the value kk to the value of register Vx, then stores the result in Vx. 
    /// ## Arguments
    /// - ``x`` register to add
    /// - ``kk`` value
    fn add(&mut self, x: u8, kk: u8){
        self.v[x as usize] += kk
    }

    /// # ``8xy0`` - LD Vx, Vy
    /// Set Vx = Vy.
    /// Stores the value of register Vy in register Vx.
    /// ## Arguments
    /// - ``x`` source register
    /// - ``y`` destination register 
    fn ld_reg_reg(&mut self, x: u8, y: u8){
        self.v[x as usize] = self.v[y as usize] 
    }

    /// # ``8xy1`` - OR Vx, Vy
    /// Set Vx = Vx OR Vy.
    /// Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx. 
    /// A bitwise OR compares the corrseponding bits from two values, 
    /// and if either bit is 1, then the same bit in the result is also 1. Otherwise, it is 0.
    /// ## Arguments
    /// - `x` Destination register
    /// - `y` Check register
    fn or(&mut self, x: u8, y: u8){

    }

    /// # ``8xy2`` - AND Vx, Vy
    /// Set Vx = Vx AND Vy.
    /// Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx. A bitwise AND compares the corrseponding bits from two values, and if both bits are 1, then the same bit in the result is also 1. Otherwise, it is 0.
    /// ## Arguments
    /// - `x` Destination register
    /// - `y` Check register
    fn and(&mut self, x: u8, y: u8){

    }

    /// # `8xy3` - XOR Vx, Vy
    /// Set Vx = Vx XOR Vy.
    /// Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result in Vx. An exclusive OR compares the corrseponding bits from two values, and if the bits are not both the same, then the corresponding bit in the result is set to 1. Otherwise, it is 0. 
    /// ## Arguments
    /// - `x` Destination register
    /// - `y` Check register
    fn xor(&mut self, x: u8, y: u8){

    }

    /// # `8xy4` - ADD Vx, Vy
    /// Set Vx = Vx + Vy, set VF = carry.
    /// The values of Vx and Vy are added together. If the result is greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.
    /// ## Argument
    /// - `x` Destination Register
    /// - `y` Operand Register
    fn add_carry(&mut self, x: u8, y: u8){

    }


    /// # `8xy5` - SUB Vx, Vy
    /// Set Vx = Vx - Vy, set VF = NOT borrow.
    /// If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx.
    /// ## Arguments
    /// - `x` Destination Register 
    /// - `y` Operand Register
    fn sub(&mut self, x: u8, y: u8){

    }

    /// # `8xy6` - SHR Vx {, Vy}
    /// Set Vx = Vx SHR 1.
    /// If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2.
    /// ## Arguments
    /// - `x` Register
    fn shr(&mut self, x: u8){

    }

    /// # `8xy7` - SUBN Vx, Vy
    /// Set Vx = Vy - Vx, set VF = NOT borrow.
    /// If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx.
    /// ## Arguments
    /// - `x` Destination Register
    /// - `y` Operand Register
    fn subn(&mut self, x: u8, y: u8){

    }

    /// # `8xyE` - SHL Vx {, Vy}
    /// Set Vx = Vx SHL 1.
    /// If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2.
    /// ## Arguments
    /// - `x` Register
    fn shl(&mut self, x: u8){

    }

    /// # `9xy0` - SNE Vx, Vy
    /// Skip next instruction if Vx != Vy.
    /// The values of Vx and Vy are compared, and if they are not equal, the program counter is increased by 2.
    /// ## Arguments
    /// - `x` First Register
    /// - `y` Second Register
    fn sne_register(&mut self, x: u8, y: u8){

    }

    /// # `Annn` - LD I, addr
    /// Set I = nnn.
    /// The value of register I is set to nnn.
    /// ## Arguments
    /// - `nnn` Address
    fn ld_i(&mut self,  nnn: u16){

    }

    /// # `Bnnn` - JP V0, addr
    /// Jump to location nnn + V0.
    /// The program counter is set to nnn plus the value of V0.
    /// ## Arguments
    /// - `nnn` Address
    fn jmp_memory(&mut self, nnn: u16){

    }

    /// # Cxkk - RND Vx, byte
    /// Set Vx = random byte AND kk.
    /// The interpreter generates a random number from 0 to 255, which is then ANDed with the value kk. The results are stored in Vx. See instruction 8xy2 for more information on AND.
    /// ## Arguments
    /// - `x` Register
    /// - `kk` Value 
    fn rnd(&mut self, x: u8, kk: u8){

    }

    /// # `Dxyn` - DRW Vx, Vy, nibble
    /// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision.
    /// The interpreter reads n bytes from memory, starting at the address stored in I. These bytes are then displayed as sprites on screen at coordinates (Vx, Vy). Sprites are XORed onto the existing screen. If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0. If the sprite is positioned so part of it is outside the coordinates of the display, it wraps around to the opposite side of the screen. See instruction 8xy3 for more information on XOR, and section 2.4, Display, for more information on the Chip-8 screen and sprites.
    /// ## Arguments
    /// - `x` Register for x coordinate
    /// - `y` Register for y coordinate
    /// - `n` bytes
    fn drw(&mut self, x: u8, y: u8, n: u8){

    }

    /// # `Ex9E` - SKP Vx
    /// Skip next instruction if key with the value of Vx is pressed.
    /// Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, PC is increased by 2.
    /// ## Arguments
    /// - `x` key value
    fn skp(&mut self, x: u8){

    }

    /// # `ExA1` - SKNP Vx  
    /// Skip next instruction if key with the value of Vx is not pressed.
    /// Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.
    /// ## Arguments
    /// - `x` key value
    fn sknp(&mut self, x: u8){

    }

    /// # `Fx07` - LD Vx, DT
    /// Set Vx = delay timer value.
    /// The value of DT is placed into Vx.
    /// ## Arguments
    /// - `x` Register
    fn ld_reg_delay(&mut self, x: u8){

    }

    /// # `Fx0A` - LD Vx, K
    /// Wait for a key press, store the value of the key in Vx.
    /// All execution stops until a key is pressed, then the value of that key is stored in Vx.
    /// ## Arguments
    /// - `x` Register
    fn ld_key(&mut self, x:u8){

    } 

    /// # `Fx15` - LD DT, Vx
    /// Set delay timer = Vx.
    /// DT is set equal to the value of Vx.
    /// ## Arguments
    /// - `x` Register
    fn ld_delay_reg(&mut self, x:u8){

    }

    /// # `Fx18` - LD ST, Vx
    /// Set sound timer = Vx.
    /// ST is set equal to the value of Vx.
    /// ## Arguments
    /// - `x` Register
    fn ld_sound(&mut self, x: u8){

    }

    /// # `Fx1E` - ADD I, Vx
    /// Set I = I + Vx.
    /// The values of I and Vx are added, and the results are stored in I.
    /// ## Arguments
    /// - `x` Register
    fn add_i(&mut self, x: u8){

    }

    /// # `Fx29` - LD F, Vx
    /// Set I = location of sprite for digit Vx.
    /// The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx. See section 2.4, Display, for more information on the Chip-8 hexadecimal font.
    /// ## Arguments
    /// - `x` Register
    fn ld_font(&mut self, x: u8){

    }

    /// # `Fx33` - LD B, Vx
    /// Store BCD representation of Vx in memory locations I, I+1, and I+2.
    /// The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.
    /// ## Arguments
    /// - `x` Register
    fn ld_bcd(&mut self, x: u8){

    }

    /// # `Fx55` - LD [I], Vx
    /// Store registers V0 through Vx in memory starting at location I.
    /// The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.
    /// ## Arguments
    /// - `x` Register
    fn ld_mem(&mut self, x: u8){

    }

    /// # `Fx65` - LD Vx, [I]
    /// Read registers V0 through Vx from memory starting at location I.
    /// The interpreter reads values from memory starting at location I into registers V0 through Vx.
    /// ## Arguments
    /// - `x` Register
    fn ld_reg_mem(&mut self, x: u8){

    }




}
