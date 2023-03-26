 mod chocolate;
 use chocolate::Chocolate;

fn main() {
    let mut cpu = Chocolate::new();
    cpu.tick(true);
}
