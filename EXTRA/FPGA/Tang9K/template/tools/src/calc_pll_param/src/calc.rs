use std::cmp::Ordering;

pub const ALLOWED_ODIV: [u64; 11] = [2, 4, 8, 16, 32, 48, 64, 80, 96, 112, 128];

#[derive(Debug, Copy, Clone)]
pub struct PllLimits {
    pub idiv_sel_min: u64,
    pub idiv_sel_max: u64,
    pub fbdiv_sel_min: u64,
    pub fbdiv_sel_max: u64,
    pub pfd_min_hz: u64,
    pub pfd_max_hz: u64,
    pub clkout_min_hz: u64,
    pub clkout_max_hz: u64,
    pub vco_min_hz: u64,
    pub vco_max_hz: u64,
}

#[derive(Debug, Copy, Clone)]
pub struct PllSolution {
    pub idiv_sel: u64,
    pub fbdiv_sel: u64,
    pub odiv_sel: u64,
    pub pfd_hz: u64,
    pub clkout_hz: u64,
    pub vco_hz: u64,
    pub error_hz: u64,
}

pub fn solve_pll(
    fin_hz: u64,
    target_hz: u64,
    limits: &PllLimits,
) -> Result<Vec<PllSolution>, String> {
    if fin_hz == 0 {
        return Err("input frequency must be greater than 0".to_string());
    }
    if target_hz == 0 {
        return Err("target frequency must be greater than 0".to_string());
    }

    let mut results = Vec::new();

    for idiv_sel in limits.idiv_sel_min..=limits.idiv_sel_max {
        let idiv = idiv_sel + 1;
        let pfd_num = fin_hz as u128;
        let div_den = idiv as u128;
        if !ratio_in_closed_range(pfd_num, div_den, limits.pfd_min_hz, limits.pfd_max_hz) {
            continue;
        }

        for fbdiv_sel in limits.fbdiv_sel_min..=limits.fbdiv_sel_max {
            let fbdiv = fbdiv_sel + 1;
            let clkout_num = fin_hz as u128 * fbdiv as u128;
            if !ratio_in_open_closed_range(
                clkout_num,
                div_den,
                limits.clkout_min_hz,
                limits.clkout_max_hz,
            ) {
                continue;
            }

            for &odiv_sel in &ALLOWED_ODIV {
                let vco_num = clkout_num * odiv_sel as u128;
                if !ratio_in_closed_range(vco_num, div_den, limits.vco_min_hz, limits.vco_max_hz) {
                    continue;
                }

                let pfd_hz = div_round(pfd_num, div_den) as u64;
                let clkout_hz = div_round(clkout_num, div_den) as u64;
                let vco_hz = div_round(vco_num, div_den) as u64;

                results.push(PllSolution {
                    idiv_sel,
                    fbdiv_sel,
                    odiv_sel,
                    pfd_hz,
                    clkout_hz,
                    vco_hz,
                    error_hz: abs_diff(clkout_hz, target_hz),
                });
            }
        }
    }

    results.sort_by(compare_solution);

    if results.is_empty() {
        Err("no valid PLL solution found".to_string())
    } else {
        Ok(results)
    }
}

fn compare_solution(a: &PllSolution, b: &PllSolution) -> Ordering {
    a.error_hz
        .cmp(&b.error_hz)
        .then_with(|| a.idiv_sel.cmp(&b.idiv_sel))
        .then_with(|| a.fbdiv_sel.cmp(&b.fbdiv_sel))
        .then_with(|| a.odiv_sel.cmp(&b.odiv_sel))
}

fn div_round(numerator: u128, denominator: u128) -> u128 {
    (numerator + denominator / 2) / denominator
}

fn ratio_in_closed_range(numerator: u128, denominator: u128, min: u64, max: u64) -> bool {
    numerator >= min as u128 * denominator && numerator <= max as u128 * denominator
}

fn ratio_in_open_closed_range(numerator: u128, denominator: u128, min: u64, max: u64) -> bool {
    numerator > min as u128 * denominator && numerator <= max as u128 * denominator
}

fn abs_diff(a: u64, b: u64) -> u64 {
    if a >= b { a - b } else { b - a }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::chip::ChipKind;

    #[test]
    fn solves_tang_nano_9k_126mhz() {
        let chip = ChipKind::TangNano9K.chip();
        let result = solve_pll(27_000_000, 126_000_000, &chip.limits).unwrap();
        let best = result[0];

        assert_eq!(best.error_hz, 0);
        assert_eq!(best.clkout_hz, 126_000_000);
        assert!(best.vco_hz >= chip.limits.vco_min_hz);
        assert!(best.vco_hz <= chip.limits.vco_max_hz);
    }

    #[test]
    fn solves_tang_nano_1k_108mhz() {
        let chip = ChipKind::TangNano1K.chip();
        let result = solve_pll(27_000_000, 108_000_000, &chip.limits).unwrap();
        let best = result[0];

        assert_eq!(best.error_hz, 0);
        assert_eq!(best.clkout_hz, 108_000_000);
        assert!(best.vco_hz >= chip.limits.vco_min_hz);
        assert!(best.vco_hz <= chip.limits.vco_max_hz);
    }
}
