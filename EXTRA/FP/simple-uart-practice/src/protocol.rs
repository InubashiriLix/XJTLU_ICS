//! practise-use protocol
//! 0xAA, LEN: u8, DATA: u8*, CHECKSUM: u8
//! The checksum means all the result of wrapping_add all the bytes of the data
#[derive(Debug)]
pub struct Packet {
    pub magic: u8,
    pub length: usize,
    pub data: Vec<u8>,
    pub checksum: u8,
}

#[derive(Debug)]
enum PacketError {
    TooShort,
    BadMagic(u8),
    InvalidLength { excepted: usize, actual: usize },
    ChecksumMismatch { expected: u8, actual: u8 },
}

fn verify_length(data: &[u8]) -> Result<usize, PacketError> {}
fn verify_magic(data: &[u8]) -> Result<u8, PacketError> {
    if data.len() < 1 {
        PacketError::TooShort
    }
    Ok(data[0])
}

fn decode_packed(data: &[u8]) -> Result<Packet, PacketError> {}

// I think it is the fucking end
