import json
import logging

SECRETS_FILE = 'static/tg_secrets.json'

logger = logging.getLogger(__name__)


class Config:
    SCRAPING = {
        'url': 'https://smart-lab.ru/dividends/',
        'get_params': {'is_approved': 1, 'upcoming': 1, 'with_dates': 1},
        'idle_request': False,
        'idle_request_filename': 'static/test_response.txt',
        'company_number': 5,
    }
    MESSAGE_INFO = {
        'required_headers': [
            'Тикер',
            'Див.',
            'Цена',
            'Див.дох.',
            'купить до',
        ],
    }
    SECRETS = {}

    def __init__(self):
        try:
            with open(SECRETS_FILE, 'r') as secrets_file:
                secrets = json.load(secrets_file)
        except Exception as exc:
            logger.warning(str(exc))
            return
        for key, val in secrets.items():
            self.SECRETS[key] = val

