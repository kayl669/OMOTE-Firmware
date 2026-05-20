import os
import subprocess

Import("env")

OTA_USER = "omote"  # same as in ota_hal_esp32.cpp


def ota_upload(source, target, env):
    firmware = str(source[0])
    host = env.GetProjectOption("upload_port", "OMOTE")
    url = f"http://{host}:3232/update"

    # this is the password which has been compiled into the firmware, see ota_password.py
    new_password = os.environ.get("OTA_PASSWORD", "")
    if not new_password:
        raise Exception("The environment variable OTA_PASSWORD is not set. "
                        "OTA would be disabled by this firmware, so it is not uploaded.")
    # to change the password, give the one of the running firmware in OTA_PASSWORD_OLD
    password = os.environ.get("OTA_PASSWORD_OLD") or new_password

    print(f"[OTA] Uploading {firmware} to {url}")
    # the password is given on stdin, so that it does not show up in the list of processes
    escaped = f"{OTA_USER}:{password}".replace("\\", "\\\\").replace('"', '\\"')
    result = subprocess.run(
        [
            "curl", "--silent", "--show-error",
            "--config", "-",
            "--write-out", "\n%{http_code} %{time_total}",
            "--form", f"firmware=@{firmware}",
            "--max-time", "180",
            url,
        ],
        input=f'user = "{escaped}"\n',
        stdout=subprocess.PIPE,
        text=True,
    )
    if result.returncode != 0:
        raise Exception(f"OTA upload failed (curl exit code {result.returncode})")

    body, _, status = result.stdout.rpartition("\n")
    code, _, seconds = status.partition(" ")
    print(f"[OTA] HTTP {code} in {seconds}s: {body}")
    if code == "401":
        raise Exception("OTA upload refused: wrong password. If it was changed, "
                        "set the environment variable OTA_PASSWORD_OLD to the old one.")
    if code != "200" or body.strip() != "OK":
        raise Exception("OTA upload failed")


env.Replace(UPLOADCMD=ota_upload)
