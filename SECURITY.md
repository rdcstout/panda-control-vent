# Security

Panda Control Vent is intended for a trusted local network. Do not expose its
HTTP interface through internet port forwarding or place it on an untrusted
public network.

Never include Wi-Fi passwords, Bambu LAN access codes, Moonraker credentials,
control tokens, printer serial numbers, or complete diagnostic logs in a public
issue.

For a vulnerability that would place other users at immediate risk, use
GitHub's private vulnerability-reporting feature when it is available for this
repository. Otherwise, publish only a redacted description sufficient to
request a private maintainer conversation.

Security fixes should preserve OTA compatibility and the ability to recover a
controller using official stock firmware.
