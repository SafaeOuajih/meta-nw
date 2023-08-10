LICENSE = "CLOSED"
inherit systemd
SYSTEMD_AUTO_ENABLE = "enable"
SYSTEMD_SERVICE_${PN} = "servicea.service"

SRC_URI = "file://service_3.c"
SRC_URI:append = " file://servicea.service "
FILES:${PN} += "\${systemd_unitdir}/system/servicea.service"
FILES:${PN} += "/lib /lib/systemd/"
S = "${WORKDIR}"

do_compile() {
	${CC} service_3.c ${LDFLAGS} -o service_3
}

do_install() {
	install -d ${D}${bindir}
	install -m 0755 service_3 ${D}${bindir}
}

do_install:append() {
  install -d \${D}/\${systemd_unitdir}/system
  install -m 0644 \${WORKDIR}/servicea.service \${D}/\${systemd_unitdir}/system
}
