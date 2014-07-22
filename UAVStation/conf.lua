-- UAV configuration file
uav = {
    ip_version=4,
    backup_file="/tmp/uav_backup",

    server = {
        port=61245,
        interface="lo",
        timeout=4000,
        info="DUAV-3XL-04205-RTPO7",
    },

    central = {
        host="127.0.0.1",
        port=5688,
    },

    videos = {
        {
            url="rtmp://<IP>:1935/flvplayback/livestream",
            info="Front video",
        },
    },
}
