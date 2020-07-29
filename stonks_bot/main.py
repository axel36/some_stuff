import logging
import requests
import typing as tp

import main_config

from bs4 import BeautifulSoup

logger = logging.getLogger(__name__)


def send_message(message: str, secrets: tp.Dict[str, str]) -> None:
    params = {
        'chat_id': secrets['chat_id'],
        'text': message,
        'parse_mode': 'html',
    }

    response = requests.get(
        f'https://api.telegram.org/bot{secrets["tg_token"]}/sendMessage',
        params=params,
    )

    if response.status_code != 200:
        print(
            'ERROR: send_message ({}, {})'.format(
                response.status_code, response.text,
            ),
        )
        exit(1)


def get_data(
        config: tp.Dict[str, str],
) -> tp.Tuple[tp.Dict[str, tp.List[str]], tp.List[str]]:
    if config['idle_request']:
        with open(config['idle_request_filename'], 'r') as f:
            resp_text = f.read()
    else:
        resp_text = requests.get(
            url=config['url'], params=config['get_params'],
        ).text

    soup = BeautifulSoup(resp_text, 'html.parser')
    table = soup.find(
        'table',
        {
            'class': (
                'fixtable moex_bonds_inline simple-little-table trades-table'
            ),
        },
    )

    table_content = table.find_all('tr', {'class': 'dividend_approved'})
    html_th_headers = table.find('thead').find_all('th')
    headers = []
    for th in html_th_headers:
        headers.append(th.text)

    companies = {}
    for row in table_content[0 : config['company_number']]:
        tds = row.find_all('td')
        info = []
        for td in tds:
            info.append(td.text.strip('\n').strip('\t'))
        companies[tds[0].text] = info

    return companies, headers


def prepare_message(
        companies_info: tp.Dict[str, tp.List[str]],
        headers: tp.List[str],
        required_headers: tp.List[str],
) -> str:
    for i, h in enumerate(headers):
        if h == 'дивиденд,руб':
            headers[i] = 'Див.'
        if h == 'Цена акции':
            headers[i] = 'Акция'

    info_result = []
    for k, v in companies_info.items():
        com = {}
        for i, el in enumerate(v):
            if headers[i] in required_headers:
                com[headers[i]] = el
        info_result.append(com)

    lines = ['|' + '|'.join(info_result[0].keys()) + '|',
             '|-----|:----:|:------:|:------:|----------:|']
    for row in info_result:
        lines.append('|' + '|'.join(row.values()) + '|')

    message = '\n'.join(lines)
    message = f'<pre>{message}</pre>'
    return message


def main(config: main_config.Config) -> None:
    if not config.SECRETS:
        logger.error('cant send message without tg secrets')
        return

    companies, headers = get_data(config.SCRAPING)
    message = prepare_message(
        companies, headers, config.MESSAGE_INFO['required_headers'],
    )

    # print(message)

    send_message(message, config.SECRETS)


if __name__ == '__main__':
    cfg = main_config.Config()
    main(cfg)
