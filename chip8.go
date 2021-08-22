package main

import (
	"fmt"
)

type chip8 struct {
	v [0xF]uint8
	i uint16

	pc uint16
	sp uint8

	delay uint8
	sound uint8

	stack  [0xF]uint16
	memory [0xFFF]uint8
	video  [64 * 32]bool
}

// Reset chip8 to defaults
func (c *chip8) Reset() {
	c.memory = [0xFFF]uint8{}
	c.v, c.stack = [0xF]uint8{}, [0xF]uint16{}
	c.video = [64 * 32]bool{}
	c.i, c.sp, c.delay, c.sound, c.NNN, c.NN, c.N, c.X, c.Y = 0, 0, 0, 0, 0, 0, 0, 0, 0
	c.pc = 0x200
}

func (c *chip8) Cycle() {

	//Fetch
	var op uint16 = uint16(c.memory[c.pc])<<8 | uint16(c.memory[c.pc+1])

	var NNN uint16 = op & 0x0FFF
	var KK uint8 = uint8(op & 0x00FF)
	var N uint8 = uint8(op & 0x000F)
	var X uint8 = uint8((op & 0x0F00) >> 8)
	var Y uint8 = uint8((op & 0x00F0) >> 4)

	//Decode & Execute
	switch op & 0xF000 {
	case 0x000:
		switch op & 0x000F {
		// 0x00E0 CLR - Clears the display
		case 0x0000:
			c.video = [64 * 32]bool{}
		// 0x00EE RET - Return from subroutine
		case 0x000E:
			c.pc = c.pop()
		}
	// 1NNN JP addr - Jump to location NNN
	case 0x1000:
		c.pc = NNN

	// 2NNN CALL addr - Call subroutine at NNN
	case 0x2000:
		c.pc++
		c.push(c.pc)
		c.pc = NNN
	// 3XKK SE Vx, byte - Skip next instruction if Vx != kk
	case 0x3000:

	case 0x4000:
		fmt.Printf("Opcode not implemented")
	case 0x5000:
		fmt.Printf("Opcode not implemented")
	case 0x6000:
		fmt.Printf("Opcode not implemented")
	case 0x7000:
		fmt.Printf("Opcode not implemented")
	case 0x8000:
		switch op & 0x000F {
		case 0x0001:
			fmt.Printf("Opcode not implemented")
		case 0x0002:
			fmt.Printf("Opcode not implemented")
		case 0x0003:
			fmt.Printf("Opcode not implemented")
		case 0x0004:
			fmt.Printf("Opcode not implemented")
		case 0x0005:
			fmt.Printf("Opcode not implemented")
		case 0x0006:
			fmt.Printf("Opcode not implemented")
		case 0x0007:
			fmt.Printf("Opcode not implemented")
		case 0x0008:
			fmt.Printf("Opcode not implemented")
		case 0x000E:
			fmt.Printf("Opcode not implemented")
		}
	case 0x9000:
		fmt.Printf("Opcode not implemented")
	case 0xA000:
		fmt.Printf("Opcode not implemented")
	case 0xB000:
		fmt.Printf("Opcode not implemented")
	case 0xC000:
		fmt.Printf("Opcode not implemented")
	case 0xD000:
		fmt.Printf("Opcode not implemented")
	case 0xE000:
		switch op & 0x00FF {
		case 0x009E:
			fmt.Printf("Opcode not implemented")
		case 0x00A1:
			fmt.Printf("Opcode not implemented")
		}
	case 0xF000:
		switch op & 0x00FF {
		case 0x0007:
			fmt.Printf("Opcode not implemented")
		case 0x000A:
			fmt.Printf("Opcode not implemented")
		case 0x0015:
			fmt.Printf("Opcode not implemented")
		case 0x0018:
			fmt.Printf("Opcode not implemented")
		case 0x001E:
			fmt.Printf("Opcode not implemented")
		case 0x0029:
			fmt.Printf("Opcode not implemented")
		case 0x0033:
			fmt.Printf("Opcode not implemented")
		case 0x0055:
			fmt.Printf("Opcode not implemented")
		case 0x0065:
			fmt.Printf("Opcode not implemented")
		}
	}

}

func (c *chip8) push(val uint16) {
	c.sp++
	c.stack[c.sp] = val

}

func (c *chip8) pop() uint16 {
	val := c.stack[c.sp]
	c.sp--
	return val
}
