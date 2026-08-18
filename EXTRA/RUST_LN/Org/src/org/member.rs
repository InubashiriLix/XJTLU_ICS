pub trait Member {
    fn name(&self) -> &str;
    fn role(&self) -> &str;
    fn age(&self) -> u8;
    fn work(&self) -> &str;
}
