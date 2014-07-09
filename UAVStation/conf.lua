-- UAV configuration file
uav = {
    ip_version=4,
    backup_file="/tmp/uav_backup",

    server = {
        port=1337,
        interface="wlp3s0",
        timeout=4000,
        info="UAV-3XL-04205-RTPO7",
    },

    central = {
        host="127.0.0.1",
        port=5875,
    },

    videos = {
        {
            url="rtmp://IP:1935/flvplayback/livestream",
            info="Front video",
        },
    },
}
