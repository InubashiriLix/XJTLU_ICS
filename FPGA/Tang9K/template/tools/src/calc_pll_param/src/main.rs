mod calc;
mod chip;

use calc::{PllSolution, solve_pll};
use chip::{Chip, ChipKind};
use std::env;
use std::error::Error;
use strum::IntoEnumIterator;

fn main() -> Result<(), Box<dyn Error>> {
    let args = env::args().skip(1).collect::<Vec<_>>();

    if args.is_empty() || args[0] == "-h" || args[0] == "--help" {
        print_usage();
        return Ok(());
    }

    if args[0] == "list" {
        print_chip_list();
        return Ok(());
    }

    if args.len() < 3 || args.len() > 4 {
        print_usage();
        return Err("invalid argument count".into());
    }

    let chip = args[0]
        .parse::<ChipKind>()
        .map_err(|_| format!("unsupported chip '{}'", args[0]))?
        .chip();
    let input_hz = parse_hz(&args[1], "input_hz")?;
    let target_hz = parse_hz(&args[2], "target_hz")?;
    let limit = match args.get(3) {
        Some(value) => value
            .parse::<usize>()
            .map_err(|e| format!("invalid result limit '{}': {}", value, e))?,
        None => 10,
    };

    if limit == 0 {
        return Err("result limit must be greater than 0".into());
    }

    let solutions = solve_pll(input_hz, target_hz, &chip.limits)?;
    print_solutions(&chip, input_hz, target_hz, &solutions, limit);

    Ok(())
}

fn print_usage() {
    println!("Usage:");
    println!("  calc_pll_param list");
    println!("  calc_pll_param <chip> <input_hz> <target_hz> [result_limit]");
    println!();
    println!("Supported chips:");
    for kind in ChipKind::iter() {
        let chip = kind.chip();
        println!("  {:<12} {} ({})", kind, chip.board_name, chip.device);
    }
    println!();
    println!("Examples:");
    println!("  calc_pll_param TangNano9K 27000000 25200000");
    println!("  calc_pll_param tn1k 27000000 100000000 5");
}

fn print_chip_list() {
    for kind in ChipKind::iter() {
        let chip = kind.chip();
        println!("{}:", kind);
        println!("  board: {}", chip.board_name);
        println!("  device: {}", chip.device);
        println!("  pll: {}", chip.pll_name);
        println!(
            "  clkout: {}..{} Hz",
            chip.limits.clkout_min_hz + 1,
            chip.limits.clkout_max_hz
        );
        println!(
            "  vco: {}..{} Hz",
            chip.limits.vco_min_hz, chip.limits.vco_max_hz
        );
    }
}

fn print_solutions<'a>(
    chip: &Chip,
    input_hz: u64,
    target_hz: u64,
    solutions: &'a [PllSolution],
    limit: usize,
) {
    println!("chip: {} ({})", chip.board_name, chip.device);
    println!("pll: {}", chip.pll_name);
    println!("input: {} Hz ({:.6} MHz)", input_hz, hz_to_mhz(input_hz));
    println!("target: {} Hz ({:.6} MHz)", target_hz, hz_to_mhz(target_hz));
    println!();
    println!(
        "{:>2} {:>8} {:>9} {:>8} {:>14} {:>14} {:>14} {:>12}",
        "#", "IDIV_SEL", "FBDIV_SEL", "ODIV_SEL", "CLKOUT_HZ", "ERROR_HZ", "PFD_HZ", "VCO_HZ"
    );

    for (index, solution) in solutions.iter().take(limit).enumerate() {
        println!(
            "{:>2} {:>8} {:>9} {:>8} {:>14} {:>14} {:>14} {:>12}",
            index + 1,
            solution.idiv_sel,
            solution.fbdiv_sel,
            solution.odiv_sel,
            solution.clkout_hz,
            solution.error_hz,
            solution.pfd_hz,
            solution.vco_hz
        );
    }

    println!();
    println!("Best rPLL parameters:");
    println!("  .FCLKIN(\"{:.6}\"),", hz_to_mhz(input_hz));
    if let Some(best) = solutions.first() {
        println!("  .IDIV_SEL({}),", best.idiv_sel);
        println!("  .FBDIV_SEL({}),", best.fbdiv_sel);
        println!("  .ODIV_SEL({})", best.odiv_sel);
    }
}

fn parse_hz(value: &str, label: &str) -> Result<u64, String> {
    let parsed = value
        .parse::<u64>()
        .map_err(|e| format!("invalid {} '{}': {}", label, value, e))?;
    if parsed == 0 {
        Err(format!("{} must be greater than 0", label))
    } else {
        Ok(parsed)
    }
}

fn hz_to_mhz(value: u64) -> f64 {
    value as f64 / 1_000_000.0
}
