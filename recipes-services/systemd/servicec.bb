LICENSE = "CLOSED"
inherit systemd
SYSTEMD_AUTO_ENABLE = "enable"
SYSTEMD_SERVICE_${PN} = "servicec.service"

SRC_URI = "file://service_2.c"
SRC_URI:append = " file://servicec.service "
FILES:${PN} += "\${systemd_unitdir}/system/servicec.service"
FILES:${PN} += "/lib /lib/systemd/"
S = "${WORKDIR}"

do_compile() {
	${CC} service_2.c ${LDFLAGS} -o service_2
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 service_2 ${D}${bindir}
}

do_install:append() {
  install -d \${D}/\${systemd_unitdir}/system
  install -m 0644 \${WORKDIR}/servicec.service \${D}/\${systemd_unitdir}/system
}
