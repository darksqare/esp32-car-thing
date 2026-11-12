Import("env")
import os
import subprocess

platform = env.PioPlatform()
esptool_dir = platform.get_package_dir("tool-esptoolpy")
framework_dir = platform.get_package_dir("framework-arduinoespressif32")

esptool_py = os.path.join(esptool_dir, "esptool.py")
boot_app0 = os.path.join(framework_dir, "tools", "partitions", "boot_app0.bin")


def merge_bin(source, target, env):
    build_dir = env.subst("$BUILD_DIR")
    bootloader = os.path.join(build_dir, "bootloader.bin")
    partitions = os.path.join(build_dir, "partitions.bin")
    firmware = str(target[0])
    merged_out = os.path.join(build_dir, "firmware-full.bin")

    cmd = [
        env.subst("$PYTHONEXE"),
        esptool_py,
        "--chip", "esp32c3",
        "merge_bin",
        "-o", merged_out,
        "--flash_mode", env.BoardConfig().get("build.flash_mode", "dio"),
        "--flash_freq", "80m",
        "--flash_size", "4MB",
        "0x0", bootloader,
        "0x8000", partitions,
        "0xe000", boot_app0,
        "0x10000", firmware,
    ]
    print("Generating full merged firmware binary:", merged_out)
    subprocess.check_call(cmd)


env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", merge_bin)
