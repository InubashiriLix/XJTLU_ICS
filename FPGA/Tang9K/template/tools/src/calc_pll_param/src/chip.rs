use crate::calc::PllLimits;
use std::fmt;
use std::str::FromStr;
use strum::{Display, EnumIter};

#[derive(Debug, Clone, Copy, EnumIter, Display)]
pub enum ChipKind {
    TangNano9K,
    TangNano1K,
}

#[derive(Debug, Clone, Copy)]
pub struct Chip {
    pub board_name: &'static str,
    pub device: &'static str,
    pub pll_name: &'static str,
    pub limits: PllLimits,
}

impl ChipKind {
    pub fn chip(self) -> Chip {
        match self {
            ChipKind::TangNano9K => Chip {
                board_name: "Tang Nano 9K",
                device: "GW1NR-9C",
                pll_name: "rPLL",
                limits: PllLimits {
                    idiv_sel_min: 0,
                    idiv_sel_max: 63,
                    fbdiv_sel_min: 0,
                    fbdiv_sel_max: 63,
                    pfd_min_hz: mhz(3),
                    pfd_max_hz: mhz(400),
                    clkout_min_hz: hz_from_mhz_frac(3_125, 1_000),
                    clkout_max_hz: mhz(600),
                    vco_min_hz: mhz(400),
                    vco_max_hz: mhz(1_200),
                },
            },
            ChipKind::TangNano1K => Chip {
                board_name: "Tang Nano 1K",
                device: "GW1NZ-1",
                pll_name: "rPLL",
                limits: PllLimits {
                    idiv_sel_min: 0,
                    idiv_sel_max: 63,
                    fbdiv_sel_min: 0,
                    fbdiv_sel_max: 63,
                    pfd_min_hz: mhz(3),
                    pfd_max_hz: mhz(400),
                    clkout_min_hz: hz_from_mhz_frac(3_125, 1_000),
                    clkout_max_hz: mhz(400),
                    vco_min_hz: mhz(400),
                    vco_max_hz: mhz(800),
                },
            },
        }
    }
}

impl FromStr for ChipKind {
    type Err = ParseChipKindError;

    fn from_str(value: &str) -> Result<Self, Self::Err> {
        let normalized = value
            .chars()
            .filter(|c| c.is_ascii_alphanumeric())
            .flat_map(|c| c.to_lowercase())
            .collect::<String>();

        match normalized.as_str() {
            "tangnano9k" | "tn9k" | "9k" => Ok(ChipKind::TangNano9K),
            "tangnano1k" | "tn1k" | "1k" => Ok(ChipKind::TangNano1K),
            _ => Err(ParseChipKindError),
        }
    }
}

#[derive(Debug, Clone, Copy)]
pub struct ParseChipKindError;

impl fmt::Display for ParseChipKindError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "unknown chip")
    }
}

impl std::error::Error for ParseChipKindError {}

const fn mhz(value: u64) -> u64 {
    value * 1_000_000
}

const fn hz_from_mhz_frac(value: u64, scale: u64) -> u64 {
    value * 1_000_000 / scale
}
