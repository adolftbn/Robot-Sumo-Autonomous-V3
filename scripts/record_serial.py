"""Record ESP32 Sumo Robot telemetry from UART into CSV files.

Example for the Scenario B 191 PWM experiment:
    python scripts/record_serial.py --port COM3 --pwm 191 --experiment b

Stop recording with Ctrl+C. Each PWM is recorded in its own folder.
"""

from __future__ import annotations

import argparse
import csv
import sys
from pathlib import Path

try:
    import serial
except ImportError as error:
    raise SystemExit("pyserial belum terpasang. Jalankan: pip install -r requirements.txt") from error


EVENT_HEADER = ["trial", "event", "t_us", "target_pwm", "rpm_left", "duration_us"]
SAMPLE_HEADER = ["trial", "t_us", "target_pwm", "rpm_left", "motor_command", "motor_pwm", "loop_us"]
TELEMETRY_HEADER = [
    "t_ms", "state", "line_mask", "dist_left_cm", "dist_right_cm",
    "rpm_left", "pwm", "motor_command", "loop_us",
]


def open_csv(path: Path, header: list[str]):
    """Open a CSV in append mode and write its header only once."""
    write_header = not path.exists() or path.stat().st_size == 0
    file = path.open("a", newline="", encoding="utf-8")
    writer = csv.writer(file)
    if write_header:
        writer.writerow(header)
        file.flush()
    return file, writer


def last_recorded_trial(event_path: Path) -> int:
    """Return the largest local trial number already saved in this PWM folder."""
    if not event_path.exists() or event_path.stat().st_size == 0:
        return 0

    largest_trial = 0
    with event_path.open("r", newline="", encoding="utf-8") as file:
        for row in csv.DictReader(file):
            try:
                largest_trial = max(largest_trial, int(row["trial"]))
            except (KeyError, TypeError, ValueError):
                continue
    return largest_trial


def main() -> None:
    parser = argparse.ArgumentParser(description="Simpan telemetri ESP32 ke CSV.")
    parser.add_argument("--port", required=True, help="Port ESP32, misalnya COM3.")
    parser.add_argument("--baud", type=int, default=230400, help="Harus sama dengan SERIAL_BAUD firmware.")
    parser.add_argument("--pwm", type=int, required=True, choices=[127, 191, 255], help="PWM target yang sedang diuji.")
    parser.add_argument("--experiment", choices=["a", "b"], default="a", help="Jenis percobaan untuk folder dan prefix data.")
    args = parser.parse_args()

    record_prefix = args.experiment.upper()
    output_dir = Path("data") / f"scenario_{args.experiment}" / f"pwm{args.pwm}"
    output_dir.mkdir(parents=True, exist_ok=True)

    event_path = output_dir / "scenario_a_events.csv"
    event_file, event_writer = open_csv(event_path, EVENT_HEADER)
    sample_file, sample_writer = open_csv(output_dir / "scenario_a_samples.csv", SAMPLE_HEADER)
    telemetry_file, telemetry_writer = open_csv(output_dir / "telemetry.csv", TELEMETRY_HEADER)
    trial_number = last_recorded_trial(event_path)
    active_trial: int | None = None

    try:
        with serial.Serial(args.port, args.baud, timeout=1) as port:
            print(f"Merekam dari {args.port} pada {args.baud} baud.")
            print(f"CSV disimpan di: {output_dir.resolve()}")
            print(f"Trial berikutnya akan dicatat sebagai trial {trial_number + 1}.")
            print("Reset ESP32 untuk memulai tiap trial. Tekan Ctrl+C untuk berhenti.")

            while True:
                raw_line = port.readline().decode("utf-8", errors="replace").strip()
                if not raw_line:
                    continue

                fields = raw_line.split(",")
                record_type = fields[0]

                if record_type == f"{record_prefix}_EVENT" and len(fields) == len(EVENT_HEADER) + 1:
                    # ESP32 emits: <experiment>_EVENT,firmware_trial,event,... . The local
                    # logger assigns trial numbers so firmware need not be
                    # re-uploaded between trials.
                    if fields[2] == "TRIAL_START":
                        trial_number += 1
                        active_trial = trial_number
                        print(f"--- Trial {active_trial} dimulai ---")
                    if active_trial is None:
                        print(f"[INFO] Event diabaikan sebelum TRIAL_START: {raw_line}")
                        continue
                    event_writer.writerow([active_trial, *fields[2:]])
                    event_file.flush()
                    print(raw_line)
                elif record_type == f"{record_prefix}_SAMPLE" and len(fields) == len(SAMPLE_HEADER) + 1:
                    if active_trial is None:
                        continue
                    sample_writer.writerow([active_trial, *fields[2:]])
                    sample_file.flush()
                elif record_type == "TEL" and len(fields) == len(TELEMETRY_HEADER) + 1:
                    telemetry_writer.writerow(fields[1:])
                    telemetry_file.flush()
                else:
                    # Informational firmware lines and malformed input are displayed,
                    # but never mixed with measured data in the CSV files.
                    print(f"[INFO] {raw_line}")
    except serial.SerialException as error:
        raise SystemExit(f"Tidak dapat membuka {args.port}: {error}") from error
    except KeyboardInterrupt:
        print("\nPerekaman dihentikan.")
    finally:
        event_file.close()
        sample_file.close()
        telemetry_file.close()


if __name__ == "__main__":
    main()
