#!/usr/bin/env python3
"""
Rapid Scale Tester - Raggiunge 20+ threads in 2 minuti
Ottimizzato per MSG_TEST_DIALOG_REQUEST (senza chiave AI)
"""

import socket
import threading
import time
import random
import argparse
from concurrent.futures import ThreadPoolExecutor
import queue

class RapidScaleTester:
    def __init__(self, host='localhost', port=8081):
        self.host = host
        self.port = port
        self.results = []
        self.active_workers = 0
        self.lock = threading.Lock()
        self.running = True
        
    def rapid_fire_worker(self, worker_id, requests_per_second):
        """Worker che fa richieste rapid-fire per saturare il server"""
        print(f"[Worker {worker_id}] Starting rapid-fire: {requests_per_second} req/s")
        
        with self.lock:
            self.active_workers += 1
        
        request_count = 0
        start_time = time.time()
        
        try:
            while self.running and time.time() - start_time < 120:  # 2 minuti max
                # Calcola intervallo tra richieste
                interval = 1.0 / requests_per_second
                
                # Fai richiesta rapidissima
                self.single_test_request(f"{worker_id}-{request_count}")
                request_count += 1
                
                # Pausa minimale (può essere anche 0 per massima velocità)
                if interval > 0.01:  # Non andare sotto 10ms
                    time.sleep(interval)
                    
        finally:
            with self.lock:
                self.active_workers -= 1
            
        elapsed = time.time() - start_time
        actual_rps = request_count / elapsed if elapsed > 0 else 0
        print(f"[Worker {worker_id}] Completed: {request_count} req in {elapsed:.1f}s ({actual_rps:.1f} req/s)")
    
    def single_test_request(self, client_id):
        """Richiesta test super veloce"""
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(5)  # Timeout corto
            sock.connect((self.host, self.port))
            
            # MSG_TEST_DIALOG_REQUEST (tipo 2) - istantaneo
            # Dati dummy minimi per velocità massima
            request = f"2|extraversion:5.0|en|test\n"
            
            start_time = time.time()
            sock.send(request.encode())
            response = sock.recv(1024).decode().strip()
            response_time = (time.time() - start_time) * 1000
            
            sock.close()
            
            with self.lock:
                self.results.append({
                    'success': True,
                    'response_time': response_time,
                    'client_id': client_id
                })
                
        except Exception as e:
            with self.lock:
                self.results.append({
                    'success': False,
                    'error': str(e),
                    'client_id': client_id
                })
    
    def run_rapid_scale_test(self):
        """Test rapido per scaling in 2 minuti"""
        print("🚀 RAPID SCALE TEST - Target: 20+ threads in 2 minutes")
        print("=" * 60)
        print("Strategy: Escalating parallel load to force rapid scaling")
        print("Time budget: 120 seconds")
        print("-" * 60)
        
        start_time = time.time()
        
        # FASE 1: Carico iniziale per prime scale-up (0-20s)
        print("\n⚡ PHASE 1: Initial burst (0-20s)")
        print("Target: Trigger first scale-up 8→12 threads")
        
        with ThreadPoolExecutor(max_workers=15) as executor:
            # 15 workers, ognuno fa 20 req/s = 300 req/s total
            futures = []
            for i in range(15):
                future = executor.submit(self.rapid_fire_worker, f"p1-{i}", 20)
                futures.append(future)
            
            # Aspetta 20 secondi o fino al primo scale-up
            time.sleep(20)
        
        elapsed = time.time() - start_time
        print(f"Phase 1 completed in {elapsed:.1f}s")
        
        if elapsed >= 115:  # Quasi finito
            self.print_final_stats()
            return
        
        # FASE 2: Carico intensificato (20-60s)  
        print(f"\n🔥 PHASE 2: Intensified load ({elapsed:.0f}-60s)")
        print("Target: Force 12→18→27 threads scaling")
        
        with ThreadPoolExecutor(max_workers=25) as executor:
            # 25 workers, ognuno fa 25 req/s = 625 req/s total
            futures = []
            for i in range(25):
                future = executor.submit(self.rapid_fire_worker, f"p2-{i}", 25)
                futures.append(future)
            
            # Aspetta fino a 60s totali
            remaining = 60 - elapsed
            if remaining > 0:
                time.sleep(remaining)
        
        elapsed = time.time() - start_time
        print(f"Phase 2 completed in {elapsed:.1f}s")
        
        if elapsed >= 115:
            self.print_final_stats()
            return
        
        # FASE 3: Carico massimo finale (60-120s)
        print(f"\n💥 PHASE 3: Maximum pressure ({elapsed:.0f}-120s)")
        print("Target: Maintain 20+ threads")
        
        with ThreadPoolExecutor(max_workers=30) as executor:
            # 30 workers, ognuno fa 30 req/s = 900 req/s total
            futures = []
            for i in range(30):
                future = executor.submit(self.rapid_fire_worker, f"p3-{i}", 30)
                futures.append(future)
            
            # Aspetta fino a 120s totali
            remaining = 120 - elapsed
            if remaining > 0:
                time.sleep(remaining)
        
        self.running = False
        total_time = time.time() - start_time
        
        print(f"\n🏁 TEST COMPLETED in {total_time:.1f}s")
        self.print_final_stats()
    
    def run_simple_burst(self, concurrent_workers=40, duration_seconds=120):
        """Test burst semplificato - carico costante alto"""
        print(f"💣 SIMPLE BURST TEST")
        print(f"Workers: {concurrent_workers}, Duration: {duration_seconds}s")
        print("=" * 60)
        
        start_time = time.time()
        
        with ThreadPoolExecutor(max_workers=concurrent_workers) as executor:
            # Ogni worker fa richieste il più velocemente possibile
            futures = []
            for i in range(concurrent_workers):
                future = executor.submit(self.continuous_requester, f"burst-{i}", duration_seconds)
                futures.append(future)
            
            # Monitora progresso ogni 10 secondi
            while time.time() - start_time < duration_seconds:
                elapsed = time.time() - start_time
                total_req = len(self.results)
                rps = total_req / elapsed if elapsed > 0 else 0
                print(f"[{elapsed:.0f}s] Requests: {total_req}, RPS: {rps:.0f}, Active workers: {self.active_workers}")
                time.sleep(10)
        
        self.running = False
        total_time = time.time() - start_time
        self.print_final_stats()
    
    def continuous_requester(self, worker_id, duration):
        """Worker che fa richieste continue per la durata specificata"""
        with self.lock:
            self.active_workers += 1
        
        start_time = time.time()
        request_count = 0
        
        try:
            while self.running and (time.time() - start_time) < duration:
                self.single_test_request(f"{worker_id}-{request_count}")
                request_count += 1
                
                # Micro-pausa per non saturare il sistema operativo
                time.sleep(0.001)  # 1ms
                
        finally:
            with self.lock:
                self.active_workers -= 1
        
        elapsed = time.time() - start_time
        rps = request_count / elapsed if elapsed > 0 else 0
        print(f"[{worker_id}] {request_count} requests in {elapsed:.1f}s ({rps:.0f} req/s)")
    
    def print_final_stats(self):
        """Stampa statistiche finali"""
        successful = [r for r in self.results if r.get('success', False)]
        failed = [r for r in self.results if not r.get('success', True)]
        
        total_requests = len(self.results)
        success_rate = len(successful) / total_requests * 100 if total_requests > 0 else 0
        
        if successful:
            response_times = [r['response_time'] for r in successful]
            avg_response = sum(response_times) / len(response_times)
            min_response = min(response_times)
            max_response = max(response_times)
        else:
            avg_response = min_response = max_response = 0
        
        print(f"\n📊 FINAL STATISTICS")
        print(f"=" * 40)
        print(f"Total requests: {total_requests}")
        print(f"Successful: {len(successful)} ({success_rate:.1f}%)")
        print(f"Failed: {len(failed)}")
        print(f"Avg response time: {avg_response:.1f}ms")
        print(f"Min/Max response: {min_response:.1f}/{max_response:.1f}ms")
        print(f"\n🎯 CHECK SERVER LOGS FOR SCALING EVENTS:")
        print(f"Look for: 'SCALING UP: X -> Y threads'")
        print(f"Target achieved: 20+ threads? Check server output!")

def main():
    parser = argparse.ArgumentParser(description='Rapid Scale Tester')
    parser.add_argument('--host', default='localhost', help='Server host')
    parser.add_argument('--port', type=int, default=8081, help='Server port')
    parser.add_argument('--mode', choices=['phases', 'burst'], default='burst',
                        help='Test mode: phases (escalating) or burst (constant high)')
    parser.add_argument('--workers', type=int, default=50, 
                        help='Concurrent workers for burst mode')
    
    args = parser.parse_args()
    
    tester = RapidScaleTester(args.host, args.port)
    
    print(f"🎯 RAPID SCALE TESTER")
    print(f"Target: {args.host}:{args.port}")
    print(f"Goal: 20+ threads in 2 minutes")
    
    try:
        if args.mode == 'phases':
            tester.run_rapid_scale_test()
        else:
            tester.run_simple_burst(args.workers, 120)
    except KeyboardInterrupt:
        print("\n⚠️ Test interrupted")
        tester.running = False

if __name__ == '__main__':
    main()
